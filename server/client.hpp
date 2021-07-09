/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelisabe <nelisabe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/13 13:58:58 by nelisabe          #+#    #+#             */
/*   Updated: 2021/07/09 16:16:52 by nelisabe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "http.hpp"

enum RecvStatus
{
	EMPTY = 0,
	NOTFINISHED,
	FINISHED	
};

class Client
{
	public:
		Client(void);
		~Client();
		
		int				getSocket(void) const;
		void			setSocket(int socket_ID);
		http::Request	&getRequest();
		http::Response	&getResponse();
		RecvStatus		getRecvStatus() const;
		void			setRecvStatus(RecvStatus status);
	private:
		Client(Client const &);

		Client	&operator=(Client const &);

		int				_socket_ID;
		http::Request	_request;
		http::Response	_response;

		RecvStatus		_recv_status;
};

#endif
