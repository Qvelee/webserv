/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelisabe <nelisabe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/21 12:35:21 by nelisabe          #+#    #+#             */
/*   Updated: 2021/07/25 21:38:30 by nelisabe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
# define CGI_HPP

# include "http.hpp"
# include "IIOController.hpp"
# include "Errors.hpp"
# include "errors.hpp"
# include <cerrno>
# include <string.h>
# include <unistd.h>
# include <stdlib.h>
# include <map>
# include <fcntl.h>
# include <sys/types.h>
# include <sys/wait.h>
# include <sstream>

using	std::string;
using	std::map;

class Cgi
{
	public:
		Cgi(const http::Request &request, const string &cgi_handler);
		virtual ~Cgi();

		enum Status
		{
			PROCESSING = 0,
			READY,
			CHUNKED
		};

		bool	Start(void);
		bool	AddCgiFdToWatch(IIOController *fd_controller) const;
		Status	ContinueIO(IIOController *fd_controller, http::Response &response);
	private:
		Cgi(const Cgi &);
		
		Cgi	&operator=(const Cgi &);

		enum State
		{
			WRITING,
			READING,
			FINISHED,
			RECVCHUNKS,
			FINCHUNKS,
			ERROR
		};

		struct CgiHeaders
		{
			string	content_length;
			string	content_type;
			string	location;
			string	status;
			bool	ended;
		};

		bool	FindVariable(const std::string &variable,\
			const map<string, string> &table);
		void	GetVariables(const http::Request &request);
		void	GetArguments(const string &script_filename);
		bool	CopyStdIO(void);
		bool	CreatePipes(void);
		bool	ExecCgi(void);
		void	RestoreStdIO(void);
		void	FillResponse(http::Response &response);
		void	FillChunkResponse(http::Response &response);
		void	FillHeaders(http::Response &response);
		bool	Write(IIOController *fd_controller);
		bool	Read(IIOController *fd_controller);
		void	AddToResponse(const char *buffer, int bytes);
		bool	CheckForChunks(void);
		bool	ParseHeaders(const string &headers);
		bool	AddHeader(const string &headers, string &add_to,\
			const string &header_name);
		bool	CheckCgiProcessExecuted(void) const;
		int		TryWaitCgiProcess(bool force_terminate = false);
		void	CreateErrorResponse(http::Response &response) const;

		State	_state;
		bool	_chunked_headers_send;
		size_t	_already_send_bytes;

		int		_fd_stdin;
		int		_fd_stdout;
		int		_fd_cgi_input[2]; // write data to _fd_sgi_input[1];
		int		_fd_cgi_output[2]; // read data from _fd_sgi_output[0];
		pid_t	_cgi_process;

		const http::Request	*_request;
		CgiHeaders			_cgi_headers;
		string				_body;

		string	_cgi_handler;
		char	**_cgi_variables;
		char	**_cgi_arguments;

		const int	_IO_BUFFER;

		class CgiVariableMissing : public std::exception
		{
			public:
				CgiVariableMissing(string variable);
				virtual ~CgiVariableMissing() throw();

				virtual const char	*what(void) const throw();
			private:
				CgiVariableMissing(void);
				string	_variable;
		};
};

#endif

