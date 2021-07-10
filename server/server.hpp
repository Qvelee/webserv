/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelisabe <nelisabe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/06 12:56:25 by nelisabe          #+#    #+#             */
/*   Updated: 2021/07/10 12:47:06 by nelisabe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include "client.hpp"

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
# define BUFFER 120000
// ---------------------

# define SUCCESS false
# define FAILURE true

typedef	struct sockaddr_in		t_sockaddr_in;
typedef	struct sockaddr			t_sockaddr;
typedef struct addrinfo 		t_addrinfo;
typedef unsigned char			uchar;

class Server
{
	public:
		Server(void);
		virtual ~Server();

		bool	Setup(int port);
		bool	Connection(void);
	private:
		Server(Server const &);

		Server &operator=(Server const &);

		bool	Error(std::string const error) const;
		bool	CreateSocket(void);
		void	AcceptNewClient(void);
		int		InitReadSet(fd_set &set);
		bool	HandleClients(fd_set const &set);
		bool	RecvData(int socket_ID, char **buffer, int *bytes_recv);
		bool	SendData(int socket_ID, char const *buffer) const;

		ushort					_serverPort;
		int						_max_connections;
		int						_socket_ID;
		t_sockaddr_in			_socket_address; // struct with address info for socket
		std::vector<Client*>	_clients;

		const int				_READ_BUFFER_SIZE;
};

#endif
