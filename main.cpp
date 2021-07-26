/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelisabe <nelisabe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/06 13:07:13 by nelisabe          #+#    #+#             */
/*   Updated: 2021/07/24 15:41:16 by nelisabe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "WebservConf.hpp"
# include "WebServer.hpp"

int		main(int argc, char **argv)
{
	WebServer 				webserver;
	config::WebserverConf	*config;

	try
	{
		argc != 2 ? config = new config::WebserverConf() :\
			config = new config::WebserverConf(argv[1]);
	}
	catch (const char *error)
	{
		std::cerr << "Error: " << error << std::endl;
		return EXIT_FAILURE;
	}
	catch (...)
	{
		std::cerr << "Error: something went wrong" << std::endl;
		return EXIT_FAILURE;
	}

	if (webserver.Setup(*config))
	{
		delete config;
		return EXIT_FAILURE;
	}
	while (true)
		if (webserver.Connection())
			return EXIT_FAILURE;
	delete config;
	return EXIT_SUCCESS;
}
