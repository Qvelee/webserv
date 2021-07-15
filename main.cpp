/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelisabe <nelisabe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/06 13:07:13 by nelisabe          #+#    #+#             */
/*   Updated: 2021/07/15 17:15:38 by nelisabe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "server.hpp"
# include "WebservConf.hpp"

// void	getPorts(config::WebserverConf config)
// {
// 	std::vector<int>	ports;

// 	std::map<int, std::map<std::string, std::map<std::string, \
// 		config::tServer *> > >::const_iterator begin = config.serverMap.begin();
// 	std::map<int, std::map<std::string, std::map<std::string, \
// 		config::tServer *> > >::const_iterator end = config.serverMap.end();
// 	while (begin != end)
// 	{
// 		ports.push_back((*begin).first);
// 		++begin;
// 	}
// }

void	GetPorts(std::vector<int> &ports)
{
	ports.push_back(SERVER_PORT);
}

int		main(int argc, char **argv)
{
	Server 					server;
	std::vector<int>		ports;
	config::WebserverConf	*config;
	
	try
	{
		argc != 2 ? config = new config::WebserverConf() :\
			config = new config::WebserverConf(argv[1]);
	}
	catch (const char *error)
	{
		std::cerr << "Error: " << error << std::endl;
		return 1;
	}
	GetPorts(ports);
	if (server.Setup(ports, *config))
	{
		delete config;
		return 2;
	}
	while (true)
	{
		if (server.Connection())
			return 3;
	}
	delete config;
	return 0;
}
