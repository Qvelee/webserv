/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebServer.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelisabe <nelisabe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/16 14:00:25 by nelisabe          #+#    #+#             */
/*   Updated: 2021/07/22 19:54:48 by nelisabe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERVER_HPP
# define WEBSERVER_HPP

# include "Server.hpp"
# include "SelectController.hpp"

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

		void	InitFdSets(void);
		bool	CheckIfSocketReady(const fd_set &set, int socket) const;

		std::vector<Server*>	_servers;
		SelectController		_fd_controller;
};

#endif
