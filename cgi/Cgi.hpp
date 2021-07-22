/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelisabe <nelisabe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/21 12:35:21 by nelisabe          #+#    #+#             */
/*   Updated: 2021/07/22 14:28:46 by nelisabe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
# define CGI_HPP

# include "http.hpp"
# include "IIOController.hpp"

# include <errno.h>
# include <string.h>
# include <unistd.h>
# include <map>
# include <fcntl.h>

using	std::string;
using	std::map;

# define SUCCESS false
# define FAILURE true

class Cgi
{
	public:
		Cgi(const http::Request &request);
		virtual ~Cgi();

		enum Status
		{
			PROCESSING,
			FINISHED,
			ERROR
		};

		bool	Start(void);
		bool	AddCgiFdToWatch(IIOController *fd_controller,\
			IIOController::IOMode mode) const;
		Status	Write(IIOController *fd_controller);
		Status	Read(IIOController *fd_controller);
	private:
		Cgi(const Cgi &);
		
		Cgi	&operator=(const Cgi &);

		bool	FindVariable(const std::string &variable,\
			const map<string, string> &table);
		void	GetVariables(const http::Request &request);
		bool	CopyStdIO(void);
		bool	CreatePipes(void);
		bool	ExecCgi(void);
		void	RestoreStdIO(void);
		bool	Error(const std::string error) const;

		int		_fd_stdin;
		int		_fd_stdout;
		int		_fd_cgi_input[2]; // write data to _fd_sgi_input[1];
		int		_fd_cgi_output[2]; // read data from _fd_sgi_output[0];

		const char	*_cgi_script;
		char		**_script_arguments;
		char		**_cgi_variables;
		string		_body;
		int			_already_send_bytes;
		string		_cgi_response;

		const int	_IO_BUFFER;
};

#endif

