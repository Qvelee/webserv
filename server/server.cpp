/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelisabe <nelisabe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/06 12:56:02 by nelisabe          #+#    #+#             */
/*   Updated: 2021/07/09 17:18:46 by nelisabe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "server.hpp"

Server::Server(void) : _socket_ID(-1), _read_buffer_size(BUFFER)
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
		return true;
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
	fd_set	read_fds;
	int		readyFds;
	int		maxFd = 0;
	
	if ((maxFd = _init_read_set(read_fds)) == -1)
		return false;
	if ((readyFds = select(maxFd + 1, &read_fds, NULL, NULL, NULL)) == -1)
		return _error("select");
	_handle_income_requests(read_fds);
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

void	Server::_accept_new_client(void)
{
	int		newSocket;
	Client	*newClient;

	newSocket = accept(_socket_ID, NULL, NULL);	
	if (newSocket == -1 && errno != EAGAIN)
		_error("cannot accept incoming connetion");
	newClient = new Client;
	newClient->setSocket(newSocket);
	_clients.push_back(newClient);
}

int		Server::_init_read_set(fd_set &set)
{
	int		maxFd;

	FD_ZERO(&set);
	FD_SET(_socket_ID, &set);
	maxFd = _socket_ID;
	for (std::vector<Client*>::iterator it = _clients.begin(); \
		it < _clients.end(); it++)
	{
		int		client_socket = (*it)->getSocket();
		FD_SET(client_socket, &set);
		if (client_socket > maxFd)
			maxFd = client_socket;
	}
	return maxFd;
}

bool	Server::_handle_income_requests(fd_set const &set)
{
	Client		*client;
	int			client_socket;
	char		*bytes_request;
	int			bytes_recv = -1;
	std::string request;
	std::string	response;

	if (FD_ISSET(_socket_ID, &set))
		_accept_new_client();
	for (auto it = _clients.begin(); it < _clients.end(); it++)
	{
		client = *it;
		client_socket = client->getSocket();
		if (FD_ISSET(client_socket, &set))
		{
			bytes_recv = -1;

			if (_recvData(client_socket, &bytes_request, &bytes_recv))
			{
				close(client_socket);
				delete client;
				_clients.erase(it--);
				continue ;
			}
			request = std::string(bytes_request, bytes_recv);
			delete bytes_request;
			if (client->getRecvStatus() == EMPTY && \
				http::parse_request(client->getRequest(), request))
			{
				client->setRecvStatus(RecvStatus(FINISHED));
				response = http::get_response(client->getRequest(), client->getResponse());
			} else
			{
				if (client->getRecvStatus() == EMPTY)
				{
					client->setRecvStatus(NOTFINISHED);
					continue ;
				}
				else
					if (http::add_body(client->getRequest(), request))
					{
						client->setRecvStatus(RecvStatus(FINISHED));
						response = http::get_response(client->getRequest(), client->getResponse());
					} else
					{
						client->setRecvStatus(NOTFINISHED);
						continue ;
					}
			}
			if (_sendData(client_socket, response.c_str()))
			{
				close(client_socket);
				delete client;
				_clients.erase(it--);
				continue ;
			}
		}
	}
	return SUCCESS;
}

bool	Server::_recvData(int socket_ID, char **buffer, int *bytes_recv)
{
	int		bytes;
	// add out_of_buffer read
	*buffer = new char[_read_buffer_size];
	if ((bytes = recv(socket_ID, *buffer, _read_buffer_size - 1, 0)) > 0)
		(*buffer)[bytes] = '\0';
	else
		return FAILURE;
	*bytes_recv = bytes;
	return SUCCESS;
}

bool	Server::_sendData(int socket_ID, char const *buffer) const
{
	int		bytes;

	if ((bytes = send(socket_ID, buffer, strlen(buffer), 0)) == -1)
	{
		std::cout << "Cannot send data" << std::endl;
		return FAILURE;
	}
	return SUCCESS;
}
