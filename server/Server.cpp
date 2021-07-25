/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelisabe <nelisabe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/06 12:56:02 by nelisabe          #+#    #+#             */
/*   Updated: 2021/07/25 12:22:12 by nelisabe         ###   ########.fr       */
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
	if (!_clients.empty())
		for (std::vector<Client*>::iterator it = _clients.begin();\
			it < _clients.end(); it++)
			delete *it;
}

Server	&Server::operator=(const Server &) { return *this; }

bool	Server::Setup(ushort port, const std::string &ip,\
	const std::map<std::string, config::tServer> &config,\
	IIOController *fd_controller)
{
	_config = config;
	_server_ip = ip;
	_fd_controller = fd_controller;
	_max_connections = MAX_CONNECTIONS;
	if ((_server_port = port) < 1024)
		return Error("port {-} is forbidden", port, false);
	if (CreateSocket() == FAILURE)
		return FAILURE;
	if (bind(_server_socket, reinterpret_cast<t_sockaddr*>(&_socket_address), \
		sizeof(_socket_address)))
		return Error("can't bind socket to port {-}", _server_port, true);
	if (listen(_server_socket, _max_connections))
		return Error("can't listen port {-}", _server_port, true);
	std::cout << "Server setup with port " << _server_port <<\
		" and IP " << _server_ip << " finished successfully." << std::endl;
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
		inet_addr(_server_ip.c_str())) == INADDR_NONE)
		return Error("IP address {-} is not valid", _server_ip, false);
	_socket_address.sin_port = htons(_server_port);
	return SUCCESS;
}

int		Server::getSocket() const { return _server_socket; }

void	Server::AddClientsSockets(void)
{
	Client	*client;
	int		client_socket;

	for (std::vector<Client*>::iterator it = _clients.begin();\
		it < _clients.end(); it++)
	{
		client = *it;
		client_socket = client->getSocket();
		switch (client->getState())
		{
			case Client::SLEEP:
				_fd_controller->AddFDToWatch(client_socket, IIOController::READ);
				break;
			case Client::RECVING:
				_fd_controller->AddFDToWatch(client_socket, IIOController::READ);
				break;
			case Client::SENDING:
				_fd_controller->AddFDToWatch(client_socket, IIOController::WRITE);
				break;
			case Client::FINISHEDRECV:
				_fd_controller->AddFDToWatch(client_socket, IIOController::WRITE);
				client->setState(Client::SENDING);
				break;
			case Client::FINISHEDSEND:
				_fd_controller->AddFDToWatch(client_socket, IIOController::READ);
				client->setState(Client::SLEEP);
				break;
			case Client::CGIPROCESSING:
				client->CgiAddFd();
				_fd_controller->AddFDToWatch(client_socket, IIOController::WRITE);
				break;
			case Client::CGICHUNKED:
				client->CgiAddFd();
				_fd_controller->AddFDToWatch(client_socket, IIOController::WRITE);
				break;
			default:
				break;
		}
	}
}

void	Server::HandleClients(void)
{
	Client		*client;
	bool		status;

	status = SUCCESS;
	if (_fd_controller->CheckIfFDReady(_server_socket, IIOController::READ))
		AcceptNewClient();
	for (std::vector<Client*>::iterator it = _clients.begin();\
		it < _clients.end(); it++)
	{
		client = *it;
		
		switch (client->getState())
		{
			case Client::SLEEP:
				status = TryRecvRequest(*client);
				break;
			case Client::RECVING:
				status = TryRecvRequest(*client);
				break;
			case Client::SENDING:
				status = TrySendResponse(*client);
				break;
			case Client::CGIPROCESSING:
				if (client->CgiProcess() == SUCCESS)
					status = TrySendResponse(*client);
				break;
			case Client::CGICHUNKED:
				status = TrySendResponse(*client);
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
		Error("can't accept incoming connetion");
	newClient = new Client(_fd_controller);
	newClient->setSocket(newSocket);
	_clients.push_back(newClient);
}

bool	Server::TryRecvRequest(Client &client)
{
	int			client_socket;
	char		*request;
	int			request_size;

	client_socket = client.getSocket();
	if (_fd_controller->CheckIfFDReady(client_socket, IIOController::READ))
	{
		if (RecvData(client_socket, &request, &request_size) == FAILURE)
		{	
			close(client_socket);
			delete &client;
			delete[] request;
			return FAILURE;
		}
		client.CreateResponse(request, request_size, _config);
		delete[] request;
	}
	return SUCCESS;
}

bool	Server::TrySendResponse(Client &client)
{
	int		client_socket;
	int		bytes;
	
	client_socket = client.getSocket();
	if (_fd_controller->CheckIfFDReady(client_socket, IIOController::WRITE))
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
		{
			if (client.getState() != Client::CGICHUNKED)
				client.setState(Client::FINISHEDSEND);
			else
				client.setState(Client::CGIPROCESSING);
			bytes = 0;
		}
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
		Error("can't send data", false);
		return -1;
	}
	return bytes;
}
