/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelisabe <nelisabe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/21 12:35:15 by nelisabe          #+#    #+#             */
/*   Updated: 2021/07/25 12:56:20 by nelisabe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cgi.hpp"

// TODO timeout скрипта
// TODO redirect?
// TODO add error pages
// TODO send file in args

Cgi::Cgi(const http::Request &request) :\
	_fd_stdin(-1), _fd_stdout(-1), _script_arguments(NULL),\
	_cgi_variables(NULL), _already_send_bytes(0),
	_IO_BUFFER(65536)
{
	_fd_cgi_input[0] = -1;
	_fd_cgi_input[1] = -1;
	_fd_cgi_output[0] = -1;
	_fd_cgi_output[1] = -1;

	_cgi_headers.ended = false;
	_chunked_headers_send = false;

	if (FindVariable("SCRIPT_NAME", request.serv_config.cgi) == FAILURE)
		throw "execept"; // TODO add exception
	if (FindVariable("SCRIPT_FILENAME", request.serv_config.cgi) == FAILURE)
		throw "execept"; // TODO add exception
	if (FindVariable("PATH_INFO", request.serv_config.cgi) == FAILURE)
		throw "execept"; // TODO add exception
	if (FindVariable("PATH_TRANSLATED", request.serv_config.cgi) == FAILURE)
		throw "execept"; // TODO add exception

	_CGI_BIN_PATH = "./cgi-bin/";
	_cgi_script = _CGI_BIN_PATH;
	_cgi_script.append((*(request.serv_config.cgi.find("SCRIPT_NAME"))).second);
	_target_file = (*(request.serv_config.cgi.find("PATH_TRANSLATED"))).second;
	GetVariables(request);
	_request = &request;
}

Cgi::Cgi(const Cgi &) : _IO_BUFFER(65536) { }

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
			delete [] _cgi_variables[i];
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
	string	key_value_pair;
	int		i = 0;
	size_t	cgi_vars_size;

	cgi_vars_size = request.serv_config.cgi.size() + 1;
	_cgi_variables = new char*[cgi_vars_size];
	for (map<string, string>::const_iterator it = request.serv_config.cgi.begin(); \
		it != request.serv_config.cgi.end(); it++)
	{
		key_value_pair.append((*it).first);
		key_value_pair.append("=");
		key_value_pair.append((*it).second);
		_cgi_variables[i] = new char[key_value_pair.length() + 1];
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
		return Error("can't replace stdin");
	if (dup2(_fd_cgi_output[1], STDOUT_FILENO) == -1)
		return Error("can't replace stdout");
	if (ExecCgi())
		return FAILURE;
	_state = WRITING;
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
		return Error("can't duplicate stdin");
	if ((_fd_stdout = dup(STDOUT_FILENO)) == -1)
		return Error("can't duplicate stdout");
	return SUCCESS;
}

bool	Cgi::ExecCgi(void)
{
	switch ((_cgi_process = fork()))
	{
		case -1:
			return Error("can't create new process");
		case 0:
			// here all open fd will duplicate (because of fork)
			// pipes fd too => need to close them. for by 3 to 1024
			// is stupid but simple way.
			// main reason: pipes will have 2 exits and 2 entrances =>
			// 	if 1 entrance will get EOF, it will not appear on exit
			// 	because of 2 open entrances;
			// 2 reason: if we have other cgi processing on server
			// 	all pipes from it will diplicate too. as result
			// 	all cgi processes will freeze;
			for (int i = 3; i < 1024; i++)
				close(i);
			execve(_cgi_script.c_str(), _script_arguments, _cgi_variables);
			exit(errno);
		default:
			// because cgi process use _fd_cgi_input[0] to read data
			// and _fd_cgi_output[1] to send data in main process we need
			// to close them
			close(_fd_cgi_input[0]);
			_fd_cgi_input[0] = -1;
			close(_fd_cgi_output[1]);
			_fd_cgi_output[1] = -1;
			RestoreStdIO();
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

bool	Cgi::AddCgiFdToWatch(IIOController *fd_controller) const
{
	if (_state == WRITING && _fd_cgi_input[1] != -1)
		fd_controller->AddFDToWatch(_fd_cgi_input[1], IIOController::IOMode::WRITE);
	else if (_state == READING && _fd_cgi_output[0] != -1)
		fd_controller->AddFDToWatch(_fd_cgi_output[0], IIOController::IOMode::READ);
	else if (_state == RECVCHUNKS && _fd_cgi_output[0] != -1)
		fd_controller->AddFDToWatch(_fd_cgi_output[0], IIOController::IOMode::READ);
	return SUCCESS;
}

Cgi::Status	Cgi::ContinueIO(IIOController *fd_controller, http::Response &response)
{
	bool	status;

	switch (_state)
	{
		case WRITING:
			status = Write(fd_controller);
			break;
		case READING:
			status = Read(fd_controller);
			break;
		case RECVCHUNKS:
			status = Read(fd_controller);
			break;
		default:
			break;
	}
	if (status == SUCCESS)
	{
		if (_state == ERROR)
		{
			CreateErrorResponse(response);
			TryWaitCgiProcess(true);
			return READY;
		}
		if (_state == FINISHED)
		{
			FillResponse(response);
			TryWaitCgiProcess(true);
			return READY;
		}
		if (_state == RECVCHUNKS)
		{
			FillChunkResponse(response);
			_body.clear();
			_chunked_headers_send = true;
			return CHUNKED;
		}
		if (_state == FINCHUNKS)
		{
			response.body.append("0\r\n\r\n");
			TryWaitCgiProcess(true);
			return READY;
		}
	}
	return PROCESSING;
}

void	Cgi::FillResponse(http::Response &response)
{
	FillHeaders(response);
	response.body = _body;
}

void	Cgi:: FillChunkResponse(http::Response &response)
{
	std::stringstream	stream;

	stream << std::hex << _body.size();
	
	if (!_chunked_headers_send)
		FillHeaders(response);
	if (_cgi_headers.ended && !_chunked_headers_send)
		response.header.insert(std::make_pair("Transfer-encoding", "chunked"));
	response.body.append(stream.str());
	response.body.append("\r\n");
	response.body.append(_body);
	response.body.append("\r\n");
}

void	Cgi::FillHeaders(http::Response &response)
{
	int		i;

	if (_cgi_headers.ended == false)
		return ;
	if (_cgi_headers.status != "")
	{
		response.code = static_cast<http::StatusCode>(atoi(_cgi_headers.status.c_str()));
		i = _cgi_headers.status.find_first_of(' ') + 1;
		while (i < _cgi_headers.status.length())
			response.status.push_back(_cgi_headers.status.at(i++));
	}
	else
	{
		response.code = static_cast<http::StatusCode>(200);
		response.status.append("OK");
	}
	if (_cgi_headers.content_length != "")
		response.header.insert(std::make_pair("Content-length",\
			_cgi_headers.content_length));
	if (_cgi_headers.content_type != "")
		response.header.insert(std::make_pair("Content-type",\
			_cgi_headers.content_type));
}

bool	Cgi::Write(IIOController *fd_controller)
{
	if (fd_controller->CheckIfFDReady(_fd_cgi_input[1], IIOController::IOMode::WRITE))
	{
		int			bytes;
		const char	*buffer = _request->body.c_str();
		
		bytes = write(_fd_cgi_input[1], &buffer[_already_send_bytes],\
			_request->body.size() - _already_send_bytes);
		if (bytes == -1)
		{
			_state = ERROR;
			return SUCCESS;
		}
		_already_send_bytes += bytes;
		if (_already_send_bytes == _request->body.size()) // data sending finished
		{
			close(_fd_cgi_input[1]);
			_fd_cgi_input[1] = -1;
			_state = READING;
			return SUCCESS;
		}
	}
	return FAILURE;
}

bool	Cgi::Read(IIOController *fd_controller)
{
	if (fd_controller->CheckIfFDReady(_fd_cgi_output[0], IIOController::IOMode::READ))
	{
		int		bytes;
		char	buffer[_IO_BUFFER];

		if ((bytes = read(_fd_cgi_output[0], buffer, _IO_BUFFER)) == -1)
		{
			_state = ERROR;
			return SUCCESS;
		}
		else if (bytes == 0)
		{
			_state = _state != RECVCHUNKS ? FINISHED : FINCHUNKS;
			return SUCCESS;
		}
		AddToResponse(buffer, bytes);
		if (CheckForChunks())
		{
			_state = RECVCHUNKS;
			return SUCCESS;
		}
		if (_cgi_headers.ended &&\
			_body.size() == atoi(_cgi_headers.content_length.c_str()))
		{
			_state = FINISHED;
			return SUCCESS;
		}
		_state = READING;
	}
	return FAILURE;
}

void	Cgi::AddToResponse(const char *buffer, int bytes)
{
	string	response_string = string(buffer, bytes);
	size_t	header_pos;
	
	if (_cgi_headers.ended != true)
	{
		if ((header_pos = response_string.find("\n\n", 0)) != string::npos)
		{
			ParseHeaders(response_string.substr(0, header_pos));
			_body.append(response_string, header_pos + 2);
			_cgi_headers.ended = true;
		}
		else if (ParseHeaders(response_string) == FAILURE)
		{
			_body.append(response_string);
			_cgi_headers.ended = true;
		}
	}
	else
		_body.append(response_string);
}

bool	Cgi::CheckForChunks(void)
{
	if (_state == RECVCHUNKS ||\
		(_cgi_headers.ended && _cgi_headers.content_length == ""))
		return true;
	return false;
}

bool	Cgi::ParseHeaders(const string &headers)
{
	size_t	position;
	size_t	header_name_len;
	bool	status = FAILURE;

	if (_cgi_headers.content_length == "")
		status = AddHeader(headers, _cgi_headers.content_length, "Content-length:");
	if (_cgi_headers.content_type == "")
		status = AddHeader(headers, _cgi_headers.content_type, "Content-type:");
	if (_cgi_headers.location == "")
		status = AddHeader(headers, _cgi_headers.location, "Location:");
	if (_cgi_headers.status == "")
		status = AddHeader(headers, _cgi_headers.status, "Status:");
	return status;
}

bool	Cgi::AddHeader(const string &headers, string &add_to,\
	const string &header_name)
{
	size_t	position;
	size_t	header_name_len;
	size_t	n_position;

	header_name_len = header_name.length();
	if ((position = headers.find(header_name)) != string::npos)
	{
		n_position = headers.find_first_of('\n', position + header_name_len);
		add_to = headers.substr(position + header_name_len,\
			n_position - position - header_name_len);
		add_to.erase(add_to.find(' '), 1);
		return SUCCESS;
	}
	return FAILURE;
}

int		Cgi::TryWaitCgiProcess(bool force_terminate)
{
	int		status;
	int		return_value;

	return_value = waitpid(_cgi_process, &status, WNOHANG);
	if (return_value == 0 && force_terminate)
	{
		if (kill(_cgi_process, SIGTERM) != 0)
			Error("can't terminate cgi process with pid {-}", _cgi_process, true);
		if ((return_value = waitpid(_cgi_process, &status, WNOHANG)) == 0)
			if (kill(_cgi_process, SIGKILL) != 0)
				Error("can't kill cgi process with pid {-}", _cgi_process, true);
	}
	if (return_value > 0)
	{
		if (WIFEXITED(status))
			return_value = WEXITSTATUS(status);
		if (WIFSIGNALED(status))
			return_value = WTERMSIG(status);
	}
	if (return_value < 0)
		Error("can't finish cgi process with pid {-}", _cgi_process, true);
	return return_value;	
}

void	Cgi::CreateErrorResponse(http::Response &response) const
{
	http::error500(*_request, response);
}
