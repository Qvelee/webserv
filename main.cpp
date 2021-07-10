/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelisabe <nelisabe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/06 13:07:13 by nelisabe          #+#    #+#             */
/*   Updated: 2021/07/10 12:40:55 by nelisabe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "server.hpp"

int		main(void)
{
	Server server;

	if (server.Setup(SERVER_PORT))
		return 1;
	while (true)
	{
		if (server.Connection())
			return 2;
	}
	return 0;
}
