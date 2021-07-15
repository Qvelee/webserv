/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelisabe <nelisabe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/06 13:07:13 by nelisabe          #+#    #+#             */
/*   Updated: 2021/07/15 13:58:51 by nelisabe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "server.hpp"
# include "WebservConf.hpp"

int		main(void)
{
	Server server;
	config::WebserverConf config("111.conf");

	if (server.Setup(SERVER_PORT, config))
		return 1;
	while (true)
	{
		if (server.Connection())
			return 2;
	}
	return 0;
}
