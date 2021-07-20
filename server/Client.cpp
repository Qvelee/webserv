/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelisabe <nelisabe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/13 14:03:29 by nelisabe          #+#    #+#             */
/*   Updated: 2021/07/20 12:21:44 by nelisabe         ###   ########.fr       */
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
	_request_string = std::string(request, requset_size);
	if (_recv_status == EMPTY && http::parse_request(_request, _request_string,\
		config))
		_recv_status = FINISHED;
	else
		if (_recv_status == EMPTY)
			_recv_status = NOTFINISHED;
		else
			if (http::add_body(_request, _request_string))
				_recv_status = FINISHED;
			else
				_recv_status = NOTFINISHED;
	if (_recv_status == FINISHED)
	{
		http::get_response(_request, _response);
		http::ResponseToString(_response, _response_string);
		_request = http::Request();
		_response = http::Response();
		_recv_status = EMPTY;
		return SUCCESS;
	}
	else
		return FAILURE;
}
