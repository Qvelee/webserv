/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelisabe <nelisabe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/16 14:00:27 by nelisabe          #+#    #+#             */
/*   Updated: 2021/07/16 15:21:18 by nelisabe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "WebServer.hpp"

WebServer::WebServer() {}

WebServer::WebServer(const WebServer &) {}

WebServer::~WebServer()
{
	if (!_servers.empty())
		for (std::vector<Server*>::iterator it = _servers.begin();\
			it < _servers.end(); it++)
			delete *it;
}

WebServer	&WebServer::operator=(const WebServer &) { return *this; }

bool		WebServer::Setup(const std::vector<ushort> ports, \
	const config::WebserverConf &config)
{
	Server	*server;

	for (std::vector<ushort>::const_iterator it = ports.begin();\
		it < ports.end(); it++)
	{
		server = new Server();
		
		if (server->Setup(*it, config) == FAILURE)
			return FAILURE;
		_servers.push_back(server);
	}
	return SUCCESS;
}

bool		WebServer::Connection()
{
	fd_set	read_fds;
	fd_set	write_fds;
	int		readyFds;
	int		maxFd;

	if ((maxFd = InitFdSets(read_fds, write_fds)) == -1)
		return Error("there is no sockets to treat");
	if ((readyFds = select(maxFd + 1, &read_fds, &write_fds, NULL, NULL)) == -1)
		return Error("select");
	for (std::vector<Server*>::iterator it = _servers.begin();\
		it < _servers.end(); it++)
		(*it)->HandleClients(read_fds, write_fds);
	return SUCCESS;
}

int			WebServer::InitFdSets(fd_set &read_fds, fd_set &write_fds)
{
	int		maxFd = -1;
	int		temp;

	FD_ZERO(&read_fds);
	FD_ZERO(&write_fds);
	for (std::vector<Server*>::iterator it = _servers.begin();\
		it < _servers.end(); it++)
	{
		FD_SET((*it)->getSocket(), &read_fds);
		maxFd = (*it)->getSocket() > maxFd ? (*it)->getSocket() : maxFd;
	}
	for (std::vector<Server*>::iterator it = _servers.begin();\
		it < _servers.end(); it++)
	{
		temp = (*it)->AddClientsSockets(read_fds, write_fds);
		maxFd = temp > maxFd ? temp : maxFd;
	}
	return maxFd;
}

bool	WebServer::Error(const std::string error) const
{
	std::cerr << "Error: " + error + ": " << strerror(errno) << std::endl;
	return FAILURE;
}
