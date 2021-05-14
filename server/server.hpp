/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelisabe <nelisabe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/06 12:56:25 by nelisabe          #+#    #+#             */
/*   Updated: 2021/05/14 17:48:58 by nelisabe         ###   ########.fr       */
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

typedef	struct sockaddr_in	t_sockaddr_in;
typedef	struct sockaddr		t_sockaddr;
typedef struct addrinfo 	t_addrinfo;
typedef unsigned char		uchar;

class Server
{
	public:
		Server(void);
		virtual ~Server();

		bool	setup(int port);
		bool	connection(void);
	private:
		Server(Server const &);

		Server &operator=(Server const &);

		bool	_error(std::string const error) const;
		bool	_create_socket(void);
		void	_accept_new_client(void);
		int		_init_read_set(fd_set &set);
		bool	_handle_income_requests(fd_set const &set);
		bool	_parse_request(char const *request, char **response) const;
		bool	_recvData(int socket_ID, char **buffer);
		bool	_sendData(int socket_ID, char const *buffer) const;

		ushort					_serverPort;
		int						_max_connections;
		int						_socket_ID;
		t_sockaddr_in			_socket_address; // struct with address info for socket
		std::vector<Client*>	_clients;

		int	const			_read_buffer_size;
};

#endif
