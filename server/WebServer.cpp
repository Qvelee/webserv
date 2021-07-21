/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelisabe <nelisabe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/16 14:00:27 by nelisabe          #+#    #+#             */
/*   Updated: 2021/07/21 20:24:54 by nelisabe         ###   ########.fr       */
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

bool		WebServer::Setup(const config::WebserverConf &config)
{
	Server	*server;

	for (std::map<int, std::string>::const_iterator it = config.getPorts().begin();\
		it != config.getPorts().end(); it++)
	{
		server = new Server();
		
		const std::map<std::string, config::tServer> server_conf = \
			config.getServerMap().at((*it).first);
		if (server->Setup((*it).first, (*it).second, server_conf, \
			&_fd_controller) == FAILURE)
			return FAILURE;
		_servers.push_back(server);
	}
	return SUCCESS;
}

bool		WebServer::Connection(void)
{
	InitFdSets();
	if (_fd_controller.Wait() == FAILURE)
		return Error("select");
	for (std::vector<Server*>::iterator it = _servers.begin();\
		it < _servers.end(); it++)
		(*it)->HandleClients();
	return SUCCESS;
}

void		WebServer::InitFdSets(void)
{
	_fd_controller.Clear();
	for (std::vector<Server*>::iterator it = _servers.begin();\
		it < _servers.end(); it++)
		_fd_controller.AddFDToWatch((*it)->getSocket(), SelectController::READ);
	for (std::vector<Server*>::iterator it = _servers.begin();\
		it < _servers.end(); it++)
		(*it)->AddClientsSockets();
}

bool	WebServer::Error(const std::string error) const
{
	std::cerr << "Error: " + error + ": " << strerror(errno) << std::endl;
	return FAILURE;
}
