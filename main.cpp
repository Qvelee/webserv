/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelisabe <nelisabe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/06 13:07:13 by nelisabe          #+#    #+#             */
/*   Updated: 2021/05/07 16:12:25 by nelisabe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "server.hpp"

int		main(void)
{
	Server server;

	if (server.setup(SERVER_PORT))
		return 1;
	if (server.connection())
		return 2;
	while (true)
	{
		uchar	*buffer;

		server.readData(&buffer);
		std::cout << "Data recieved: " << std::endl;
		std::cout << buffer << std::endl;
		delete buffer;
		
		std::string	message;
		std::cout << "Input your message to client: ";
		std::cin >> message;
		server.sendData(message.c_str());
	}
	return 0;
}
