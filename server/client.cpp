/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelisabe <nelisabe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/13 14:03:29 by nelisabe          #+#    #+#             */
/*   Updated: 2021/07/14 17:03:41 by nelisabe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "client.hpp"

Client::Client(void) : \
	_socket_ID(-1), _recv_status(EMPTY), _connection_state(SLEEP),\
	_bytes_already_send(0) { }

Client::Client(Client const &) {}

Client::~Client(void) {}

Client	&Client::operator=(Client const &) {return *this; }

int		Client::getSocket(void) const { return _socket_ID; }

void	Client::setSocket(int socket_ID) { _socket_ID = socket_ID; }

const std::string	&Client::getResponse() const { return _response_string; }

const Client::State	Client::getState() const { return _connection_state; }

void	Client::setState(Client::State state) { _connection_state = state; }

int		Client::getAlreadySendBytes() const { return _bytes_already_send; }

void	Client::setAlreadySendBytes(int bytes) { _bytes_already_send = bytes; }

bool	Client::CreateResponse(const char *request, int requset_size)
{
	_request_string = std::string(request, requset_size);
	if (_recv_status == EMPTY && http::parse_request(_request, _request_string))
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
		_response_string = http::get_response(_request, _response);
		return SUCCESS;
	}
	else
		return FAILURE;
}
