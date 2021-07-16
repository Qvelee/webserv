/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelisabe <nelisabe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/06 12:56:02 by nelisabe          #+#    #+#             */
/*   Updated: 2021/07/16 15:20:31 by nelisabe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Server.hpp"

Server::Server(void) : _server_socket(-1), _IO_BUFFER_SIZE(65536)
{
	memset(&_socket_address, 0, sizeof(_socket_address));
}

Server::Server(const Server &) : _IO_BUFFER_SIZE(0) {}

Server::~Server()
{
	if (_server_socket != -1)
		close(_server_socket);
}

Server	&Server::operator=(const Server &) { return *this; }

bool	Server::Setup(ushort port, const config::WebserverConf &config)
{
	_config = &config;
	_max_connections = MAX_CONNECTIONS;
	if ((_server_port = port) < 1024)
		return Error("Set port is forbidden");
	if (CreateSocket() == FAILURE)
		return FAILURE;
	if (bind(_server_socket, reinterpret_cast<t_sockaddr*>(&_socket_address), \
		sizeof(_socket_address)))
		return Error("cannot bind socket to port");
	if (listen(_server_socket, _max_connections))
		return Error("cannot listen port");
	std::cout << "Server setup with port " << _server_port <<\
		" finished successfully." << std::endl;
	return SUCCESS;
}

bool	Server::CreateSocket(void)
{
	int		set_value = 1;

	// AF_INET - IPv4, SOCK_STREAM - TCP protocol
	if ((_server_socket = socket(AF_INET, SOCK_STREAM, 0)) == -1)
		return Error("failed to create socket");
	// SOL_SOCKET - protocol lvl is socket
	// SO_REUSEADDR - let socket be reusable
	if (setsockopt(_server_socket, SOL_SOCKET, SO_REUSEADDR, \
		&set_value, sizeof(set_value)) == -1)
	{
		close(_server_socket);
		return Error("failed to set socket options");
	}
	_socket_address.sin_family = AF_INET;
	if ((_socket_address.sin_addr.s_addr = \
		INADDR_ANY) == INADDR_NONE)
		return Error("Non valid IP address");
	_socket_address.sin_port = htons(_server_port);
	return SUCCESS;
}

int		Server::getSocket() const { return _server_socket; }

int		Server::AddClientsSockets(fd_set &read_fds, fd_set &write_fds)
{
	Client	*client;
	int		client_socket;
	int		maxFd = 0;

	for (std::vector<Client*>::iterator it = _clients.begin();\
		it < _clients.end(); it++)
	{
		client = *it;
		client_socket = client->getSocket();
		switch (client->getState())
		{
			case Client::State::SLEEP:
				FD_SET(client_socket, &read_fds);
				break;
			case Client::State::RECVING:
				FD_SET(client_socket, &read_fds);
				break;
			case Client::State::SENDING:
				FD_SET(client_socket, &write_fds);
				break;
			case Client::State::FINISHEDRECV:
				FD_SET(client_socket, &write_fds);
				client->setState(Client::State::SENDING);
				break;
			case Client::State::FINISHEDSEND:
				FD_SET(client_socket, &read_fds);
				client->setState(Client::State::SLEEP);
				break;
			default:
				break;
		}
		if (client_socket > maxFd)
			maxFd = client_socket;
	}
	return maxFd;
}

void	Server::HandleClients(const fd_set &read_fds, const fd_set &write_fds)
{
	Client		*client;
	bool		status;

	status = SUCCESS;
	if (FD_ISSET(_server_socket, &read_fds))
		AcceptNewClient();
	for (std::vector<Client*>::iterator it = _clients.begin();\
		it < _clients.end(); it++)
	{
		client = *it;
		
		switch (client->getState())
		{
			case Client::State::SLEEP:
				status = TryRecvRequest(*client, read_fds);
				break;
			case Client::State::RECVING:
				status = TryRecvRequest(*client, read_fds);
				break;
			case Client::State::SENDING:
				status = TrySendResponse(*client, write_fds);
				break;
			default:
				break;
		}
		if (status == FAILURE)
			_clients.erase(it--);
	}
}

void	Server::AcceptNewClient(void)
{
	int		newSocket;
	Client	*newClient;

	newSocket = accept(_server_socket, NULL, NULL);	
	if (newSocket == -1 && errno != EAGAIN)
		Error("cannot accept incoming connetion");
	newClient = new Client;
	newClient->setSocket(newSocket);
	_clients.push_back(newClient);
}

bool	Server::TryRecvRequest(Client &client, const fd_set &read_fds)
{
	int			client_socket;
	char		*request;
	int			request_size;

	client_socket = client.getSocket();
	if (FD_ISSET(client_socket, &read_fds))
	{
		if (RecvData(client_socket, &request, &request_size) == FAILURE)
		{	
			close(client_socket);
			delete &client;
			return FAILURE;
		}
		if (client.CreateResponse(request, request_size, *_config) == SUCCESS)
			client.setState(Client::State::FINISHEDRECV);
		else
			client.setState(Client::State::RECVING);
		delete request;
	}
	return SUCCESS;
}

bool	Server::TrySendResponse(Client &client, const fd_set &write_fds)
{
	int		client_socket;
	int		bytes;
	
	client_socket = client.getSocket();
	if (FD_ISSET(client_socket, &write_fds))
	{
		if ((bytes = SendData(client_socket, client.getResponse().c_str(),\
			client.getResponse().size(), client.getAlreadySendBytes())) == -1)
		{
			close(client_socket);
			delete &client;
			return FAILURE;
		}
		bytes += client.getAlreadySendBytes();
		if (bytes == client.getResponse().size())
			client.setState(Client::State::FINISHEDSEND);
		client.setAlreadySendBytes(bytes);
	}
	return SUCCESS;
}

bool	Server::RecvData(int socket, char **buffer, int *buffer_size)
{
	*buffer = new char[_IO_BUFFER_SIZE];
	if ((*buffer_size = recv(socket, *buffer, _IO_BUFFER_SIZE - 1, 0)) > 0)
		(*buffer)[*buffer_size] = '\0';
	else
		return FAILURE;
	return SUCCESS;
}

int		Server::SendData(int socket, const char *buffer,\
	int buffer_size, int start_pos) const
{
	int		bytes;

	if ((bytes = send(socket, &buffer[start_pos],\
		buffer_size - start_pos, MSG_DONTWAIT)) == -1)
	{
		std::cout << "cannot send data" << std::endl;
		return -1;
	}
	return bytes;
}

bool	Server::Error(const std::string error) const
{
	std::cerr << "Error: " + error + ": " << strerror(errno) << std::endl;
	return FAILURE;
}
