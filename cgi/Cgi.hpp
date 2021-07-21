/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelisabe <nelisabe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/21 12:35:21 by nelisabe          #+#    #+#             */
/*   Updated: 2021/07/21 17:49:13 by nelisabe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
# define CGI_HPP

# include "http.hpp"
# include <errno.h>
# include <string.h>
# include <unistd.h>
# include <map>

using	std::string;
using	std::map;

# define SUCCESS false
# define FAILURE true

class Cgi
{
	public:
		Cgi(const http::Request &request);
		virtual ~Cgi();

		bool	Start(void);
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
		int		_fd_cgi_input[2];
		int		_fd_cgi_output[2];

		const char	*_cgi_script;
		char		**_script_arguments;
		char		**_cgi_variables;
};

#endif

