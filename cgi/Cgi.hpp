/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelisabe <nelisabe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/21 12:35:21 by nelisabe          #+#    #+#             */
/*   Updated: 2021/07/23 18:09:10 by nelisabe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
# define CGI_HPP

# include "http.hpp"
# include "IIOController.hpp"
# include "Errors.hpp"
# include "errors.hpp"
# include <errno.h>
# include <string.h>
# include <unistd.h>
# include <map>
# include <fcntl.h>
# include <sys/types.h>
# include <sys/wait.h>

using	std::string;
using	std::map;

class Cgi
{
	public:
		Cgi(const http::Request &request);
		virtual ~Cgi();

		bool	Start(void);
		bool	AddCgiFdToWatch(IIOController *fd_controller) const;
		bool	ContinueIO(IIOController *fd_controller, http::Response &response);
	private:
		Cgi(const Cgi &);
		
		Cgi	&operator=(const Cgi &);

		enum State
		{
			WRITING,
			READING,
			FINISHED,
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
		bool	CopyStdIO(void);
		bool	CreatePipes(void);
		bool	ExecCgi(void);
		void	RestoreStdIO(void);
		void	FillResponse(http::Response &response);
		bool	Write(IIOController *fd_controller);
		bool	Read(IIOController *fd_controller);
		void	AddToResponse(const char *buffer, int bytes);
		bool	ParseHeaders(const string &headers);
		bool	AddHeader(const string &headers, string &add_to,\
			const string &header_name);
		int		TryWaitCgiProcess(bool force_terminate = false);
		void	CreateErrorResponse(http::Response &response) const;

		State	_state;

		int		_fd_stdin;
		int		_fd_stdout;
		int		_fd_cgi_input[2]; // write data to _fd_sgi_input[1];
		int		_fd_cgi_output[2]; // read data from _fd_sgi_output[0];
		pid_t	_cgi_process;
		string	_target_file;

		http::Request	_request;
		CgiHeaders		_cgi_headers;
		string			_body;

		const char	*_cgi_script;
		char		**_script_arguments;
		char		**_cgi_variables;
		int			_already_send_bytes;

		const int	_IO_BUFFER;
};

#endif

