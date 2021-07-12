/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbenny <bbenny@student.21-school.ru>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/24 21:50:14 by bbenny            #+#    #+#             */
/*   Updated: 2021/04/24 21:50:16 by bbenny           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cgi.hpp"

namespace config{

Cgi::Cgi()
{
	param.insert(std::make_pair("AUTH_TYPE", "Basic"));
	param.insert(std::make_pair("CONTENT_LENGTH", "-1"));
	param.insert(std::make_pair("CONTENT_TYPE", "text/html"));
	param.insert(std::make_pair("GATEWAY_INTERFACE", "CGI/1.1"));
	param.insert(std::make_pair("PATH_INFO", ""));
	param.insert(std::make_pair("PATH_TRANSLATED", ""));
	param.insert(std::make_pair("QUERY_STRING", ""));
	param.insert(std::make_pair("REMOTE_ADDR", ""));
	param.insert(std::make_pair("REMOTE_IDENT", ""));
	param.insert(std::make_pair("REMOTE_USER", ""));
	param.insert(std::make_pair("REQUEST_METHOD", ""));
	param.insert(std::make_pair("REQUEST_URI", ""));
	param.insert(std::make_pair("SCRIPT_NAME", ""));
	param.insert(std::make_pair("SERVER_NAME", ""));
	param.insert(std::make_pair("SERVER_PORT", ""));
	//param.insert(std::make_pair("SERVER_PROTOCOL", ""));
	//param.insert(std::make_pair("SERVER_SOFTWARE", ""));
}
Cgi::~Cgi(){}
Cgi::Cgi(Cgi const &copy){*this = copy;}
Cgi &Cgi::operator=(Cgi const &eq)
{
	param = eq.param;
	return (*this);
}

}
