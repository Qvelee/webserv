/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelisabe <nelisabe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/13 14:03:29 by nelisabe          #+#    #+#             */
/*   Updated: 2021/07/24 22:49:39 by nelisabe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Client.hpp"

Client::Client(IIOController *fd_controller) : \
	_client_socket(-1), _recv_status(EMPTY), _connection_state(SLEEP),\
	_bytes_already_send(0), _cgi(NULL)
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

const std::string	&Client::getResponse() const { return _response_string; }

const Client::State	Client::getState() const { return _connection_state; }

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
	_request.serv_config.cgi.insert(std::make_pair("PATH_INFO", "/cgi-test.txt"));
	_request.serv_config.cgi.insert(std::make_pair("PATH_TRANSLATED",\
		"/home/guplee/42/webserv/cgi-test.txt"));
	_request.serv_config.cgi.insert(std::make_pair("SCRIPT_NAME", "cpptest"));
	_request.serv_config.cgi.insert(std::make_pair("SCRIPT_FILENAME",\
		"/home/guplee/42/webserv/cgi-bin/cpptest"));
	// _request.serv_config.cgi.insert(std::make_pair("PATH_INFO", "/cgi-test.txt"));
	// _request.serv_config.cgi.insert(std::make_pair("PATH_TRANSLATED",\
		// "/home/guplee/42/webserv/cgi-test.txt"));
	// _request.serv_config.cgi.insert(std::make_pair("SCRIPT_NAME", "cppchunkedtest"));
	// _request.serv_config.cgi.insert(std::make_pair("SCRIPT_FILENAME",\
		// "/home/guplee/42/webserv/cgi-bin/cppchunkedtest"));

	_cgi = new Cgi(_request);
	if (_cgi->Start() == FAILURE)
	{
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
		if (_response.code != http::StatusCode::NoError)
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
