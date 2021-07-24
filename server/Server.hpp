/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelisabe <nelisabe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/06 12:56:25 by nelisabe          #+#    #+#             */
/*   Updated: 2021/07/24 11:48:15 by nelisabe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "Client.hpp"
# include "IIOController.hpp"

# include <iostream>
# include <string>
# include <cstring>
# include <vector>

# include <sys/socket.h>
# include <sys/select.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <unistd.h>
# include <errno.h>
# include <fcntl.h>

// -- server settings --
# define SERVER_PORT 8080
# define MAX_CONNECTIONS 5
// ---------------------

typedef	struct sockaddr_in		t_sockaddr_in;
typedef	struct sockaddr			t_sockaddr;
typedef struct addrinfo 		t_addrinfo;
typedef unsigned char			uchar;

class Server
{
	public:
		Server(void);
		virtual ~Server();

		bool	Setup(ushort port, const std::string &ip,\
			const std::map<std::string, config::tServer> &config,\
			IIOController *fd_controller);
		void	AddClientsSockets(void);
		void	HandleClients(void);

		int		getSocket() const;
	private:
		Server(const Server &);

		Server &operator=(const Server &);

		bool	CreateSocket(void);
		void	AcceptNewClient(void);
		bool	TryRecvRequest(Client &client);
		bool	TrySendResponse(Client &client);
		bool	RecvData(int socket_ID, char **buffer, int *bytes_recv);
		int		SendData(int socket_ID, const char *buffer,\
			int buffer_size, int start_pos) const;

		std::map<std::string, config::tServer>	_config;

		int						_max_connections;
		ushort					_server_port;
		int						_server_socket;
		std::string				_server_ip;
		t_sockaddr_in			_socket_address; // struct with address info for socket
		std::vector<Client*>	_clients;
		IIOController			*_fd_controller;

		const int				_IO_BUFFER_SIZE;
};

#endif
