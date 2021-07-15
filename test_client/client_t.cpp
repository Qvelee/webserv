/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client_t.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelisabe <nelisabe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/05 17:40:26 by nelisabe          #+#    #+#             */
/*   Updated: 2021/07/14 18:38:28 by nelisabe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "client_t.hpp"

int		create_socket(void)
{
	int		socket_ID;

	if ((socket_ID = socket(AF_INET, SOCK_STREAM, 0)) == -1)
	{
		std::cout << "Error crearing socket\n";
		return -1;
	}
	int		set_value = 1;
	if (setsockopt(socket_ID, SOL_SOCKET, SO_REUSEADDR, \
		&set_value, sizeof(set_value)) == -1)
	{
		std::cout << "Error set socket options\n";
		return -1;
	}
	return socket_ID;
}

bool	parse_request(char const *reqest, char **response)
{
	if (reqest)
	{
		std::cout << "Request: ";
		std::cout << reqest << std::endl << "ENDL" << std::endl;
	}
	
	// std::cout << "Create your response: ";
	std::string	resp = "Hello World!";
	// std::getline(std::cin, resp);

	// std::cout << resp.c_str() << std::endl;
	*response = new char[resp.size() + 1];
	memcpy(*response, resp.c_str(), resp.size() + 1);
	return false;
}

int		main(void)
{
	int		socket_ID = create_socket();

	if (socket_ID == -1)
		return 1;
	t_sockaddr_in	server;

	memset(&server, 0, sizeof(server));
	server.sin_family = AF_INET;
	if ((server.sin_addr.s_addr = inet_addr("192.168.151.100")) == -1)
	{
		std::cout << "Error inet_addr\n";
		close(socket_ID);
		return 1;
	}
	server.sin_port = htons(8080);
	if (connect(socket_ID, (sockaddr*)&server, sizeof(server)) == -1)
	{
		std::cout << "Error connect " << strerror(errno) << std::endl;
		close(socket_ID);
		return 1;
	}
	std::cout << "connected\n";
		int		response_size = 10;
		char	response[response_size];
		
		// parse_request(NULL, &response);
		
		std::cout << "Sending data\n";
		memset(response, 'a', response_size);
		send(socket_ID, response, response_size, 0);
		std::cout << "Data send\n";

		int		buffer_size = 250000;
		char	buffer[buffer_size];
		
		int bytes;
		int total = 0;
		while (bytes != 0)
		{
			if ((bytes = recv(socket_ID, buffer, buffer_size - 1, 0)) == -1)
			{
				std::cout << "Error recv\n";
				return 1;
			}
			std::cout << "Data recv " << bytes << "\n";
			total += bytes;
			// if (!bytes)
			// 	break ;
			// std::cout << "Bytes recieved: " << bytes << std::endl;
			std::cout << buffer << std::endl;
		}
		std::cout << total << std::endl;
		// std::string	stop;
		// std::getline(std::cin, stop);
		// if (stop == "stop")
		// 	break;

	close(socket_ID);
	return 0;
}
