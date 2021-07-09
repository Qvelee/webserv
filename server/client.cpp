/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelisabe <nelisabe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/13 14:03:29 by nelisabe          #+#    #+#             */
/*   Updated: 2021/07/09 16:17:49 by nelisabe         ###   ########.fr       */
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

http::Request	&Client::getRequest() { return _request; }

http::Response	&Client::getResponse() { return _response; }

RecvStatus	Client::getRecvStatus() const { return _recv_status; }

void		Client::setRecvStatus(RecvStatus status) { _recv_status = status; }
