/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelisabe <nelisabe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/05/13 13:58:58 by nelisabe          #+#    #+#             */
/*   Updated: 2021/07/26 11:35:18 by nelisabe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

# include "http.hpp"
# include "Cgi.hpp"
# include "IIOController.hpp"
# include "Errors.hpp"
# include <sstream>

using	std::string;

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
			CGIPROCESSING,
			CGICHUNKED
		};

		void	CreateResponse(const char *request, int request_size,\
			const std::map<string, config::tServer> &config);

		int				getSocket(void) const;
		void			setSocket(int socket_ID);
		const string	&getClientPort(void);
		void			setClientPort(ushort port);
		const string	&getClientIp(void);
		void			setClientIp(const string &ip);
		void			setServerPort(ushort port);
		void			setServerIp(const string &ip);
		const string	&getResponse() const;
		State			getState() const;
		void			setState(State state);
		int				getAlreadySendBytes() const;
		void			setAlreadySendBytes(int bytes);
		size_t			getLastRequestTime(void);
		void			setLastRequestTime(size_t time);
		void			CgiAddFd(void) const;
		bool			CgiProcess(void);
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
		string			_client_port;
		string			_client_ip;
		string			_server_port;
		string			_server_ip;
		string			_request_string;
		string			_response_string;
		http::Request	_request;
		http::Response	_response;
		int				_bytes_already_send;
		Cgi				*_cgi;
		IIOController	*_fd_controller;
		size_t			_last_request;

		RecvStatus		_recv_status;
		State			_connection_state;
	};

#endif
