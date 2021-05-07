/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelisabe <nelisabe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/06 12:56:02 by nelisabe          #+#    #+#             */
/*   Updated: 2021/05/07 15:40:17 by nelisabe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "server.hpp"

Server::Server(void) : _socket_ID(-1), _read_buffer_size(64000)
{
	memset(&_socket_address, 0, sizeof(_socket_address));
}

Server::Server(Server const &) : _read_buffer_size(0) {}

Server::~Server()
{
	if (_socket_ID != -1)
		close(_socket_ID);
}

Server	&Server::operator=(Server const &) { return *this; }

bool	Server::setup(int port)
{
	_max_connections = MAX_CONNECTIONS;
	if ((_serverPort = port) < 1024)
		return _error("Set port is forbidden");
	if (_create_socket())
		return 2;
	if (bind(_socket_ID, reinterpret_cast<t_sockaddr*>(&_socket_address), \
		sizeof(_socket_address)))
		return _error("cannot bind socket to port");
	if (listen(_socket_ID, _max_connections))
		return _error("cannot listen port");
	std::cout << "Server setup finished successfully." << std::endl;
	return false;
}

bool	Server::connection(void)
{
	t_addrinfo	client_address;
	uint		addrinfo_size = sizeof(client_address);

	if ((_client_socket_ID = accept(_socket_ID, \
		reinterpret_cast<t_sockaddr*>(&client_address), \
		reinterpret_cast<socklen_t*>(&addrinfo_size))) == -1)
		return _error("cannot accept incoming connection");
	return false;
}

bool	Server::readData(uchar **buffer)
{
	int		bytes;

	*buffer = new uchar[_read_buffer_size];
	if ((bytes = recv(_client_socket_ID, *buffer, _read_buffer_size - 1, 0)) == -1)
		return _error("cannot read data"); // forbidden by subject
	if (!bytes)
		close(_client_socket_ID);
	(*buffer)[bytes] = '\0';
	std::cout << "Bytes readed: " << bytes << std::endl;
	return false;
}

bool	Server::sendData(char const *buffer) const
{
	int		bytes;

	if ((bytes = send(_client_socket_ID, buffer, strlen(buffer), 0)) == -1)
		return _error("cannot send data");
	return false;
}

bool	Server::_error(std::string const error) const
{
	std::cerr << "Error: " + error + ": " << strerror(errno) << std::endl;
	return true;
}

bool	Server::_create_socket(void)
{
	int		set_value = 1;

	// AF_INET - IPv4, SOCK_STREAM - TCP protocol
	if ((_socket_ID = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		return _error("failed to create socket");

	// SOL_SOCKET - protocol lvl is socket
	// SO_REUSEADDR - let socket be reusable
	if (setsockopt(_socket_ID, SOL_SOCKET, SO_REUSEADDR, \
		&set_value, sizeof(set_value)) == -1)
	{
		close(_socket_ID);
		return _error("failed to set socket options");
	}

	_socket_address.sin_family = AF_INET;
	if ((_socket_address.sin_addr.s_addr = \
		INADDR_ANY) == INADDR_NONE)
		return _error("Non valid IP address");
	_socket_address.sin_port = htons(_serverPort);
	return false;
}
