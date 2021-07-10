/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelisabe <nelisabe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/06 12:56:02 by nelisabe          #+#    #+#             */
/*   Updated: 2021/07/10 14:53:40 by nelisabe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "server.hpp"

Server::Server(void) : _socket_ID(-1), _READ_BUFFER_SIZE(BUFFER)
{
	memset(&_socket_address, 0, sizeof(_socket_address));
}

Server::Server(const Server &) : _READ_BUFFER_SIZE(0) {}

Server::~Server()
{
	if (_socket_ID != -1)
		close(_socket_ID);
}

Server	&Server::operator=(const Server &) { return *this; }

bool	Server::Setup(int port)
{
	_max_connections = MAX_CONNECTIONS;
	if ((_serverPort = port) < 1024)
		return Error("Set port is forbidden");
	if (CreateSocket())
		return true;
	if (bind(_socket_ID, reinterpret_cast<t_sockaddr*>(&_socket_address), \
		sizeof(_socket_address)))
		return Error("cannot bind socket to port");
	if (listen(_socket_ID, _max_connections))
		return Error("cannot listen port");
	std::cout << "Server setup finished successfully." << std::endl;
	return false;
}

bool	Server::CreateSocket(void)
{
	int		set_value = 1;

	// AF_INET - IPv4, SOCK_STREAM - TCP protocol
	if ((_socket_ID = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		return Error("failed to create socket");
	// SOL_SOCKET - protocol lvl is socket
	// SO_REUSEADDR - let socket be reusable
	if (setsockopt(_socket_ID, SOL_SOCKET, SO_REUSEADDR, \
		&set_value, sizeof(set_value)) == -1)
	{
		close(_socket_ID);
		return Error("failed to set socket options");
	}
	_socket_address.sin_family = AF_INET;
	if ((_socket_address.sin_addr.s_addr = \
		INADDR_ANY) == INADDR_NONE)
		return Error("Non valid IP address");
	_socket_address.sin_port = htons(_serverPort);
	return false;
}

bool	Server::Connection(void)
{
	fd_set	read_fds;
	int		readyFds;
	int		maxFd = 0;
	
	if ((maxFd = InitReadSet(read_fds)) == -1)
		return false;
	if ((readyFds = select(maxFd + 1, &read_fds, NULL, NULL, NULL)) == -1)
		return Error("select");
	HandleClients(read_fds);
	return false;
}

int		Server::InitReadSet(fd_set &set)
{
	int		maxFd;

	FD_ZERO(&set);
	FD_SET(_socket_ID, &set);
	maxFd = _socket_ID;
	for (std::vector<Client*>::iterator it = _clients.begin();\
		it < _clients.end(); it++)
	{
		int		client_socket = (*it)->getSocket();

		FD_SET(client_socket, &set);
		if (client_socket > maxFd)
			maxFd = client_socket;
	}
	return maxFd;
}

bool	Server::HandleClients(const fd_set &set)
{
	Client		*client;
	int			client_socket;
	char		*request;
	int			request_size;

	if (FD_ISSET(_socket_ID, &set))
		AcceptNewClient();
	for (std::vector<Client*>::iterator it = _clients.begin();\
		it < _clients.end(); it++)
	{
		client = *it;
		client_socket = client->getSocket();
		if (FD_ISSET(client_socket, &set))
		{
			if (RecvData(client_socket, &request, &request_size))
			{	
				close(client_socket);
				delete client;
				_clients.erase(it--);
			} else if (client->CreateResponse(request, request_size))
				delete request;
			else if (SendData(client_socket,\
				client->getResponse().c_str(), client->getResponse().size()))
			{
				close(client_socket);
				delete client;
				_clients.erase(it--);
			}
		}
	}
	return SUCCESS;
}

void	Server::AcceptNewClient(void)
{
	int		newSocket;
	Client	*newClient;

	newSocket = accept(_socket_ID, NULL, NULL);	
	if (newSocket == -1 && errno != EAGAIN)
		Error("cannot accept incoming connetion");
	newClient = new Client;
	newClient->setSocket(newSocket);
	_clients.push_back(newClient);
}

bool	Server::RecvData(int socket_ID, char **buffer, int *buffer_size)
{
	*buffer = new char[_READ_BUFFER_SIZE];
	if ((*buffer_size = recv(socket_ID, *buffer, _READ_BUFFER_SIZE - 1, 0)) > 0)
		(*buffer)[*buffer_size] = '\0';
	else
		return FAILURE;
	return SUCCESS;
}

bool	Server::SendData(int socket_ID, const char *buffer, int buffer_size) const
{
	int		bytes;

	if ((bytes = send(socket_ID, buffer, buffer_size, 0)) == -1)
	{
		std::cout << "Cannot send data" << std::endl;
		return FAILURE;
	}
	return SUCCESS;
}

bool	Server::Error(const std::string error) const
{
	std::cerr << "Error: " + error + ": " << strerror(errno) << std::endl;
	return true;
}
