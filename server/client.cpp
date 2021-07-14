/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelisabe <nelisabe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/13 14:03:29 by nelisabe          #+#    #+#             */
/*   Updated: 2021/07/10 14:57:47 by nelisabe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "client.hpp"

Client::Client(void) : _socket_ID(-1)
{
	_recv_status = RecvStatus(EMPTY);
}

Client::Client(Client const &) {}

Client::~Client(void) {}

Client	&Client::operator=(Client const &) {return *this; }

int		Client::getSocket(void) const { return _socket_ID; }

void	Client::setSocket(int socket_ID) { _socket_ID = socket_ID; }

const std::string	&Client::getResponse() const { return _response_string; }

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
