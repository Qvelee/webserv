/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelisabe <nelisabe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/06 12:56:02 by nelisabe          #+#    #+#             */
/*   Updated: 2021/05/06 17:08:26 by nelisabe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "server.hpp"

Server::Server(void) : _socket_ID(-1)
{
	memset(&_socket_address, 0, sizeof(_socket_address));
}

Server::Server(Server const &) {}

Server::~Server()
{
	if (_socket_ID != -1)
		close(_socket_ID);
}

Server	&Server::operator=(Server const &) { return *this; }

int		Server::setup(int port)
{
	_max_connections = MAX_CONNECTIONS;
	if ((_serverPort = port) < 1024)
		return _error("Set port is forbidden", 1);
	if (_create_socket())
		return 2;
	if (bind(_socket_ID, reinterpret_cast<t_sockaddr*>(&_socket_address), \
		sizeof(_socket_address)))
		return _error("cannot bind socket to port", 4);
	if (listen(_socket_ID, _max_connections))
		return _error("cannot listen port", 5);
	std::cout << "Server setup finished successfully." << std::endl;
	return 0;
}

int		Server::connection(void)
{
	
	return 0;
}

int		Server::_error(std::string const error, int ret) const
{
	std::cerr << "Error: " + error + ": " << strerror(errno) << std::endl;
	return ret;
}

int		Server::_create_socket(void)
{
	int		set_value = 1;

	// AF_INET - IPv4, SOCK_STREAM - TCP protocol
	if ((_socket_ID = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		return _error("failed to create socket", 1);

	// SOL_SOCKET - protocol lvl is socket
	// SO_REUSEADDR - let socket be reusable
	if (setsockopt(_socket_ID, SOL_SOCKET, SO_REUSEADDR, \
		&set_value, sizeof(set_value)) == -1)
	{
		close(_socket_ID);
		return _error("failed to set socket options", 2);
	}

	_socket_address.sin_family = AF_INET;
	if ((_socket_address.sin_addr.s_addr = \
		inet_addr("0.0.0.0")) == INADDR_NONE)
		return _error("Non valid IP address", 3);
	_socket_address.sin_port = htons(_serverPort);
	return 0;
}
