/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelisabe <nelisabe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/13 13:58:58 by nelisabe          #+#    #+#             */
/*   Updated: 2021/07/21 16:34:48 by nelisabe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "http.hpp"
# include "Cgi.hpp"

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

		bool		CreateResponse(const char *request, int request_size,\
			const std::map<std::string, config::tServer> &config);

		int					getSocket(void) const;
		void				setSocket(int socket_ID);
		const std::string	&getResponse() const;
		const State			getState() const;
		void				setState(State state);
		int					getAlreadySendBytes() const;
		void				setAlreadySendBytes(int bytes);
	private:
		Client(Client const &);

		Client	&operator=(Client const &);

		enum RecvStatus
		{
			EMPTY = 0,
			NOTFINHEADER,
			NOTFINBODY,
			FINISHED
		};

		int				_client_socket;
		int				_client_port;
		std::string		_request_string;
		std::string		_response_string;
		http::Request	_request;
		http::Response	_response;
		int				_bytes_already_send;
		Cgi				*_cgi;

		RecvStatus		_recv_status;
		State			_connection_state;
	};

#endif
