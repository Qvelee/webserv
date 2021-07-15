/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelisabe <nelisabe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/13 13:58:58 by nelisabe          #+#    #+#             */
/*   Updated: 2021/07/15 16:45:42 by nelisabe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "http.hpp"

# define SUCCESS false
# define FAILURE true

class Client
{
	public:
		Client(void);
		virtual ~Client();
		
		enum State
		{
			SLEEP = 0,
			RECVING,
			SENDING,
			FINISHEDRECV,
			FINISHEDSEND,
			ERROR,
			CLOSED
		};

		int					getSocket(void) const;
		void				setSocket(int socket_ID);
		const std::string	&getResponse() const;
		const State			getState() const;
		void				setState(State state);
		int					getAlreadySendBytes() const;
		void				setAlreadySendBytes(int bytes);

		bool		CreateResponse(const char *request, int request_size,\
			const config::WebserverConf &config);
	private:
		Client(Client const &);

		Client	&operator=(Client const &);

		enum RecvStatus
		{
			EMPTY = 0,
			NOTFINISHED,
			FINISHED	
		};

		int				_client_socket;
		int				_client_port;
		std::string		_request_string;
		std::string		_response_string;
		http::Request	_request;
		http::Response	_response;
		int				_bytes_already_send;

		RecvStatus		_recv_status;
		State			_connection_state;
};

#endif
