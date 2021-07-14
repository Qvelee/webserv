/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client_t.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelisabe <nelisabe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/06 12:56:25 by nelisabe          #+#    #+#             */
/*   Updated: 2021/07/11 13:15:58 by nelisabe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

# include <iostream>
# include <string>
# include <cstring>
# include <vector>

# include <sys/socket.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <netdb.h>
# include <unistd.h>
# include <errno.h>
# include <fcntl.h>

typedef	struct sockaddr_in	t_sockaddr_in;
typedef	struct sockaddr		t_sockaddr;
typedef struct addrinfo 	t_addrinfo;
typedef unsigned int		uint;
typedef unsigned short		ushort;
typedef unsigned char		uchar;

#endif
