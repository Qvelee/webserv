/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelisabe <nelisabe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/06 12:56:25 by nelisabe          #+#    #+#             */
/*   Updated: 2021/05/06 17:04:26 by nelisabe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <string>
# include <cstring>

# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <unistd.h>
# include <errno.h>

// -- server settings --
# define SERVER_PORT 8080
# define MAX_CONNECTIONS 5
// ---------------------

typedef	struct sockaddr_in	t_sockaddr_in;
typedef	struct sockaddr		t_sockaddr;
typedef struct addrinfo 	t_addrinfo;
typedef unsigned int		uint;
typedef unsigned short		ushort;

class Server
{
	public:
		Server(void);
		virtual ~Server();

		int		setup(int port);
		int		connection(void);
	private:
		Server(Server const &);

		Server &operator=(Server const &);

		int		_error(std::string const error, int ret) const;
		int		_create_socket(void);

		ushort			_serverPort;
		int				_max_connections;
		int				_socket_ID;
		// struct containing address information for socket
		t_sockaddr_in	_socket_address;
};

#endif
