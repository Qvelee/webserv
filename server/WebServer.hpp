/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelisabe <nelisabe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/16 14:00:25 by nelisabe          #+#    #+#             */
/*   Updated: 2021/07/16 15:38:07 by nelisabe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

#include "Server.hpp"

class WebServer
{
	public:
		WebServer();
		virtual ~WebServer();

		bool	Setup(const config::WebserverConf &config);
		bool	Connection(void);
	private:
		WebServer(const WebServer &);
		
		WebServer	&operator=(const WebServer &);

		bool	Error(const std::string error) const;
		int		InitFdSets(fd_set &read_fds, fd_set &write_fds);
		bool	CheckIfSocketReady(const fd_set &set, int socket) const;

		std::vector<Server*>	_servers;
		config::WebserverConf	*_configs;
};

#endif
