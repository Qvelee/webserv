/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelisabe <nelisabe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/04 11:56:47 by nelisabe          #+#    #+#             */
/*   Updated: 2021/05/04 16:47:20 by nelisabe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "main.hpp"

int		get_info_about_domain(const char *host)
{
	int			socket_ID;
	t_addrinfo	socket_info;
	t_addrinfo	*result_addresses;

	bzero(&socket_info, sizeof(socket_info));
	socket_info.ai_family = AF_INET;		// IPv4 address
	socket_info.ai_socktype = SOCK_STREAM;	// connection type - TCP
	socket_info.ai_flags = AI_PASSIVE;		// host IP address will set auto.

	int		status;
	if ((status = getaddrinfo(host, NULL, &socket_info, &result_addresses)))
	{
		std::cout << "Error: " << gai_strerror(status) << std::endl;
		return -1;
	}
	std::cout << "IP addresses for \"" << host << "\"" << std::endl;
	for (t_addrinfo *list = result_addresses; list; list = list->ai_next)
	{
		if (list->ai_family == AF_INET)
		{
			t_sockaddr	*socket_address = reinterpret_cast<t_sockaddr*>(list->ai_addr);
			char		address[INET_ADDRSTRLEN];

			inet_ntop(list->ai_family, &socket_address->sin_addr.s_addr, address, sizeof(address));
			std::cout << "IPv4 address: ";
			std::cout << address << std::endl;
		}
		else
			std::cout << "IPv6 address (unsupported)" << std::endl;
	}
	freeaddrinfo(result_addresses);
	return 0;
}

int		main(int argc, char **argv)
{
	if (argc != 2)
		return 1;
	get_info_about_domain(argv[1]);
	return 0;
}