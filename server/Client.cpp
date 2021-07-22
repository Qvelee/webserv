/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelisabe <nelisabe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/13 14:03:29 by nelisabe          #+#    #+#             */
/*   Updated: 2021/07/21 19:12:24 by nelisabe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Client.hpp"

Client::Client(void) : \
	_client_socket(-1), _recv_status(EMPTY), _connection_state(SLEEP),\
	_bytes_already_send(0) { }

Client::Client(Client const &) {}

Client::~Client(void) {}

Client	&Client::operator=(Client const &) {return *this; }

int		Client::getSocket(void) const { return _client_socket; }

void	Client::setSocket(int socket_ID) { _client_socket = socket_ID; }

const std::string	&Client::getResponse() const { return _response_string; }

const Client::State	Client::getState() const { return _connection_state; }

void	Client::setState(Client::State state) { _connection_state = state; }

int		Client::getAlreadySendBytes() const { return _bytes_already_send; }

void	Client::setAlreadySendBytes(int bytes) { _bytes_already_send = bytes; }

bool	Client::CreateResponse(const char *request, int requset_size,\
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
		// CGI object creating and call
		// if (true)//_request.serv_config.is_cgi)
		// {
		// 	_request.serv_config.cgi.insert(std::make_pair("PATH_TRANSLATED",\
		// 		"/home/guplee/42/webserv/cgi-bin/cpptest"));
		// 	_cgi = new Cgi(_request);
		// 	if (_cgi->Start() == FAILURE)
		// 	{
		// 		delete _cgi;
		// 		return FAILURE;
		// 	}
		// }
		http::get_response(_request, _response);
		http::ResponseToString(_response, _response_string);
		_request = http::Request();
		_response = http::Response();
		_recv_status = EMPTY;
		return SUCCESS;	
	}
	return FAILURE;
}
