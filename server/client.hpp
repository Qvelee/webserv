/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelisabe <nelisabe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/13 13:58:58 by nelisabe          #+#    #+#             */
/*   Updated: 2021/05/14 17:32:38 by nelisabe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "http.hpp"

class Client
{
	public:
		Client(void);
		~Client();
		
		int		getSocket(void) const;
		void	setSocket(int socket_ID);
	private:
		Client(Client const &);

		Client	&operator=(Client const &);

		int				_socket_ID;
		http::Request	_request;
		http::Response	_response;
};

#endif
