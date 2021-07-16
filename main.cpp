/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelisabe <nelisabe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/06 13:07:13 by nelisabe          #+#    #+#             */
/*   Updated: 2021/07/16 15:36:58 by nelisabe         ###   ########.fr       */
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
		return 1;
	}

	if (webserver.Setup(*config))
	{
		delete config;
		return 2;
	}
	while (true)
		if (webserver.Connection())
			return 3;
	delete config;
	return 0;
}
