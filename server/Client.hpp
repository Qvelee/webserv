/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelisabe <nelisabe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/13 13:58:58 by nelisabe          #+#    #+#             */
/*   Updated: 2021/07/22 12:02:46 by nelisabe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "http.hpp"
# include "Cgi.hpp"
# include "IIOController.hpp"

# define SUCCESS false
# define FAILURE true

class Client
{
	public:
		Client(IIOController *fd_controller);
		virtual ~Client();
		
		enum State
		{
			SLEEP = 0,
			RECVING,
			SENDING,
			FINISHEDRECV,
			FINISHEDSEND,
			CGISENDING,
			CGIRECVING
		};

		void	CreateResponse(const char *request, int request_size,\
			const std::map<std::string, config::tServer> &config);

		int					getSocket(void) const;
		void				setSocket(int socket_ID);
		const std::string	&getResponse() const;
		const State			getState() const;
		void				setState(State state);
		int					getAlreadySendBytes() const;
		void				setAlreadySendBytes(int bytes);
		void				CgiAddFd(IIOController::IOMode mode) const;
		void				CgiSend(void);
		void				CgiRecv(void);
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

		bool	InitCgi(void);

		int				_client_socket;
		int				_client_port;
		std::string		_request_string;
		std::string		_response_string;
		http::Request	_request;
		http::Response	_response;
		int				_bytes_already_send;
		Cgi				*_cgi;
		IIOController	*_fd_controller;

		RecvStatus		_recv_status;
		State			_connection_state;
	};

#endif
