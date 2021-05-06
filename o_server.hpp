/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   o_server.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelisabe <nelisabe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/04 11:36:11 by nelisabe          #+#    #+#             */
/*   Updated: 2021/05/06 12:55:45 by nelisabe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef MAIN_HPP
# define MAIN_HPP

# include <iostream>
# include <string>

# include <sys/socket.h>
# include <sys/time.h>
# include <netinet/in.h>
# include <unistd.h>
# include <stdlib.h>
# include <string.h>

# include <netdb.h>
# include <arpa/inet.h>

typedef	struct sockaddr_in	t_sockaddr;
typedef struct addrinfo 	t_addrinfo;

# define SERVER_PORT "8080"
# define MAX_CONNECTIONS 5

#endif
