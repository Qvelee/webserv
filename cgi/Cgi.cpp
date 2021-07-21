/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelisabe <nelisabe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/21 12:35:15 by nelisabe          #+#    #+#             */
/*   Updated: 2021/07/21 19:12:04 by nelisabe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cgi.hpp"

Cgi::Cgi(const http::Request &request) : _fd_stdin(-1), _fd_stdout(-1),\
	_cgi_script(NULL), _script_arguments(NULL), _cgi_variables(NULL)
{
	_fd_cgi_input[0] = -1;
	_fd_cgi_input[1] = -1;
	_fd_cgi_output[0] = -1;
	_fd_cgi_output[1] = -1;

	if (FindVariable("PATH_TRANSLATED", request.serv_config.cgi) == FAILURE)
		throw "execept"; // TODO add exception
	_cgi_script = (*(request.serv_config.cgi.find("PATH_TRANSLATED"))).second.c_str();
	
	GetVariables(request);
}

Cgi::Cgi(const Cgi &) { }

Cgi::~Cgi()
{
	RestoreStdIO();
	if (_fd_cgi_input[0] != -1)
		close(_fd_cgi_input[0]);
	if (_fd_cgi_input[1] != -1)
		close(_fd_cgi_input[1]);
	if (_fd_cgi_output[0] != -1)
		close(_fd_cgi_output[0]);
	if (_fd_cgi_input[1] != -1)
		close(_fd_cgi_input[1]);
	if (_cgi_variables != NULL)
	{
		int		i = -1;
		
		while (_cgi_variables[++i] != NULL)
			delete _cgi_variables[i];
		delete [] _cgi_variables;
	}
}

Cgi &Cgi::operator=(Cgi const &) { return *this; }

bool	Cgi::FindVariable(const std::string &variable,\
	const map<string, string> &table)
{
	if (table.count(variable) == 0)
		return FAILURE;
	return SUCCESS;
}

void	Cgi::GetVariables(const http::Request &request)
{
	int		i = 0;
	string	key_value_pair;

	_cgi_variables = new char*[request.serv_config.cgi.size() + 1];
	for (map<string, string>::const_iterator it = request.serv_config.cgi.begin(); \
		it != request.serv_config.cgi.end(); it++)
	{
		key_value_pair.append((*it).first);
		key_value_pair.append("=");
		key_value_pair.append((*it).second);
		_cgi_variables[i] = new char[key_value_pair.length()];
		strcpy(_cgi_variables[i], key_value_pair.c_str());
		i++;
		key_value_pair.clear();
	}
	_cgi_variables[i] = NULL;
}

bool	Cgi::Start(void)
{
	if (CopyStdIO() == FAILURE)
		return FAILURE;
	if (CreatePipes() == FAILURE)
		return FAILURE;
	if (dup2(_fd_cgi_input[0], STDIN_FILENO) == -1)
		return Error("can't dup2() fds");
	if (dup2(_fd_cgi_output[1], STDOUT_FILENO) == -1)
		return Error("can't dup2() fds");
	if (ExecCgi())
		return FAILURE;
	return SUCCESS;
}

bool	Cgi::CreatePipes(void)
{
	if (pipe(_fd_cgi_input) == -1)
		return Error("can't create pipe");
	if (pipe(_fd_cgi_output) == -1)
		return Error("can't create pipe");
	return SUCCESS;
}

bool	Cgi::CopyStdIO(void)
{
	if ((_fd_stdin = dup(STDIN_FILENO)) == -1)
		return Error("can't dup() fd");
	if ((_fd_stdout = dup(STDOUT_FILENO)) == -1)
		return Error("can't dup() fd");
	return SUCCESS;
}

bool	Cgi::ExecCgi(void)
{
	switch (fork())
	{
		case -1:
			return Error("can't create new process");
		case 0:
			execve(_cgi_script, _script_arguments, _cgi_variables);
			exit(errno);
		default:
			RestoreStdIO();
			// write(_fd_cgi_input[1], )
			// write data to _fd_sgi_input[1];
			// read data from _fd_sgi_output[0];
			break;
	}
	return SUCCESS;
}

void	Cgi::RestoreStdIO(void)
{
	if (_fd_stdin != -1)
	{
		if (dup2(_fd_stdin, STDIN_FILENO) == -1)
			Error("can't restore standart input");
		close(_fd_stdin);
		_fd_stdin = -1;
	}
	if (_fd_stdout != -1)
	{
		if (dup2(_fd_stdout, STDOUT_FILENO) == -1)
			Error("can't restore standart output");
		close(_fd_stdout);
		_fd_stdout = -1;
	}
}

bool	Cgi::Error(const std::string error) const
{
	std::cerr << "Error: " + error + ": " << strerror(errno) << std::endl;
	return FAILURE;
}
