/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelisabe <nelisabe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/13 14:03:29 by nelisabe          #+#    #+#             */
/*   Updated: 2021/07/25 21:47:18 by nelisabe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Client.hpp"

Client::Client(IIOController *fd_controller) : \
	_client_socket(-1), _bytes_already_send(0),\
	_cgi(NULL), _recv_status(EMPTY), _connection_state(SLEEP)
{
	_fd_controller = fd_controller;
}

Client::Client(Client const &) {}

Client::~Client(void)
{
	if (_cgi)
		delete _cgi;
}

Client	&Client::operator=(Client const &) {return *this; }

int		Client::getSocket(void) const { return _client_socket; }

void	Client::setSocket(int socket_ID) { _client_socket = socket_ID; }

void	Client::setClientPort(ushort port)
{
	std::stringstream	stream;

	stream << port;
	_client_port = stream.str();
}

void	Client::setClientIp(const string &ip) { _client_ip = ip; }

void	Client::setServerPort(ushort port)
{
	std::stringstream	stream;

	stream << port;
	_server_port = stream.str();
}

void	Client::setServerIp(const string &ip) { _server_ip = ip; }

const std::string	&Client::getResponse() const { return _response_string; }

Client::State	Client::getState() const { return _connection_state; }

void	Client::setState(State state) { _connection_state = state; }

int		Client::getAlreadySendBytes() const { return _bytes_already_send; }

void	Client::setAlreadySendBytes(int bytes) { _bytes_already_send = bytes; }

void	Client::CreateResponse(const char *request, int requset_size,\
	const std::map<std::string, config::tServer> &config)
{
	if (_recv_status == EMPTY || _recv_status == NOTFINHEADER)
	{
		_request_string.append(std::string(request, requset_size));
		if (_request_string.find("\r\n\r\n", 0) != std::string::npos)
		{
			if (http::parse_request(_request, _request_string, config))
				_recv_status = FINISHED;
			else
			{
				_request_string.clear();
				_recv_status = NOTFINBODY;
			}
		}
		else
			_recv_status = NOTFINHEADER;
	}
	else if (_recv_status == NOTFINBODY)
	{
		_request_string = std::string(request, requset_size);
		if (http::add_body(_request, _request_string))
			_recv_status = FINISHED;
	}
	if (_recv_status == FINISHED)
	{
		_response_string.clear();
		_request_string.clear();
		if (_request.serv_config.is_cgi)
			if (InitCgi() == SUCCESS)
			{
				_connection_state = CGIPROCESSING;
				return ;
			}
		http::get_response(_request, _response);
		http::ResponseToString(_response, _response_string);
		_request = http::Request();
		_response = http::Response();
		_recv_status = EMPTY;
		_connection_state = FINISHEDRECV;
		return ;
	}
	_connection_state = RECVING;
}

bool	Client::InitCgi(void)
{
	_request.serv_config.cgi.insert(std::make_pair("SERVER_PORT", _server_port));
	_request.serv_config.cgi.insert(std::make_pair("SERVER_ADDR", _server_ip));
	_request.serv_config.cgi.insert(std::make_pair("REMOTE_PORT", _client_port));
	_request.serv_config.cgi.insert(std::make_pair("REMOTE_ADDR", _client_ip));
	_request.serv_config.cgi.insert(std::make_pair("REDIRECT_STATUS", "200"));
	try { _cgi = new Cgi(_request, _request.serv_config.cgi_handler); }
	catch(std::exception ex)
	{
		std::cerr << ex.what() << std::endl;
		http::error500(_request, _response);
		return FAILURE;
	}
	if (_cgi->Start() == FAILURE)
	{
		http::error500(_request, _response);
		delete _cgi;
		_cgi = NULL;
		return FAILURE;
	}
	return SUCCESS;
}

void	Client::CgiAddFd(void) const
{
	_cgi->AddCgiFdToWatch(_fd_controller);
}

bool	Client::CgiProcess(void)
{
	Cgi::Status	status;

	status = _cgi->ContinueIO(_fd_controller, _response);
	if (status == Cgi::PROCESSING)
		return FAILURE;
	if (status == Cgi::READY)
	{
		_response_string.clear();
		if (_response.code != http::NoError)
			http::ResponseToString(_response, _response_string);
		else
			_response_string = _response.body;
		delete _cgi;
		_cgi = NULL;
		_recv_status = EMPTY;
		_request = http::Request();
		_response = http::Response();
		_connection_state = SENDING;
		return SUCCESS;
	}
	if (status == Cgi::CHUNKED)
	{
		_response_string.clear();
		if (!_response.header.empty())
			http::ResponseToString(_response, _response_string);
		else
			_response_string = _response.body;
		_response = http::Response();
		_connection_state = CGICHUNKED;
		return SUCCESS;
	}
	return FAILURE;
}
