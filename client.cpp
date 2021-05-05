/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelisabe <nelisabe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/05 17:40:26 by nelisabe          #+#    #+#             */
/*   Updated: 2021/05/05 17:43:05 by nelisabe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "client.hpp"

int		create_socket(t_addrinfo *address)
{
	int		socket_ID;

	if ((socket_ID = socket(address->ai_family, address->ai_socktype, address->ai_protocol)))
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

int		setup_connection(const char *port)
{
	int			socket_ID = 1;
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
	for (t_addrinfo *list = result_addresses; list; list = list->ai_next)
		if ((socket_ID = create_socket(list)))
			break ;
	if (socket_ID == -1)
	{
		freeaddrinfo(result_addresses);
		close(socket_ID);
		return 2;
	}
	std::cout << "Success" << std::endl;

	read_data();
	
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
