/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   o_server.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelisabe <nelisabe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/04 11:56:47 by nelisabe          #+#    #+#             */
/*   Updated: 2021/05/06 14:27:12 by nelisabe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "o_server.hpp"

int		read_data(int socket_ID)
{
	int		buffer_size = 52000;
	char	buffer[buffer_size];
	int		bytes;
	
	if ((bytes = recv(socket_ID, buffer, buffer_size - 1, 0)) < 0)
	{
		std::cout << "Error: recv " << strerror(errno) << std::endl;
		return 1;
	}
	buffer[bytes] = '\0';
	std::cout << "Bytes readed: " << bytes << std::endl;
	std::cout << "Data readed: " << buffer << std::endl;
	return 0;
}

int		send_data(int socket_ID)
{
	std::string	message;

	std::cout << "Print your message: ";
	std::cin >> message;
	std::cout << std::endl;

	send(socket_ID, message.c_str(), message.size(), 0);
	return 0;
}

int		create_socket(t_addrinfo *address)
{
	int		socket_ID;

	if ((socket_ID = \
		socket(address->ai_family, address->ai_socktype, address->ai_protocol)))
	{
		int		set_value = 1;

		if (setsockopt(socket_ID, SOL_SOCKET, SO_REUSEADDR, &set_value, sizeof(set_value)) == -1)
		{
			std::cerr << "Error setsockopt(): " << strerror(errno) << std::endl;
			close(socket_ID);
			return -1;
		}
		return socket_ID;
	}
	return -1;
}

int		setup_connection(const char *port)
{
	int			socket_ID;
	t_addrinfo	socket_info;
	t_addrinfo	*result_addresses;

	bzero(&socket_info, sizeof(socket_info));
	socket_info.ai_family = AF_INET;		// IPv4 address
	socket_info.ai_socktype = SOCK_STREAM;	// connection type - TCP
	socket_info.ai_flags = AI_PASSIVE;		// host IP address will set auto.

	int		status;
	if ((status = getaddrinfo(NULL, port, &socket_info, &result_addresses)))
	{
		std::cerr << "Error: " << gai_strerror(status) << std::endl;
		return 1;
	}
	t_sockaddr	*socket_address = reinterpret_cast<t_sockaddr*>(result_addresses->ai_addr);
	char		address[INET_ADDRSTRLEN];

	inet_ntop(result_addresses->ai_family, &socket_address->sin_addr.s_addr, address, sizeof(address));
	std::cout << "IPv4: " << address << std::endl;

	for (t_addrinfo *list = result_addresses; list; list = list->ai_next)
		if ((socket_ID = create_socket(list)))
			break ;
	if (socket_ID == -1)
	{
		freeaddrinfo(result_addresses);
		close(socket_ID);
		return 2;
	}
	if (bind(socket_ID, result_addresses->ai_addr, result_addresses->ai_addrlen) == -1)
	{
		std::cerr << "Error bind(): " << strerror(errno) << std::endl;
		freeaddrinfo(result_addresses);
		close(socket_ID);
		return 3;
	}
	if (listen(socket_ID, MAX_CONNECTIONS))
	{
		std::cerr << "Error listen(): " << strerror(errno) << std::endl;
		freeaddrinfo(result_addresses);
		close(socket_ID);
		return 4;
	}
	std::cout << "Success" << std::endl;

	int				new_peer;
	t_addrinfo		peer_address;
	unsigned int	address_length = sizeof(peer_address);
	if ((new_peer = accept(socket_ID, (struct sockaddr*)&peer_address, (socklen_t*)&address_length)) == -1)
	{
		std::cerr << "Error accept(): " << strerror(errno) << std::endl;
		freeaddrinfo(result_addresses);
		close(socket_ID);
		return 4;
	}

	read_data(new_peer);
	// send_data(new_peer);
	
	freeaddrinfo(result_addresses);
	close(socket_ID);
	close(new_peer);
	return 0;
}

int		main(void)
{
	if (setup_connection(SERVER_PORT))
		return 1;
	return 0;
}
