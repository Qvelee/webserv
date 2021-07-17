/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbenny <bbenny@student.21-school.ru>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/23 12:40:02 by bbenny            #+#    #+#             */
/*   Updated: 2021/04/23 12:40:04 by bbenny           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Location.hpp"
#include <sstream>

namespace config{

Location::Location()
{
	//autoindex = "off";
	accepted_methods.insert(std::make_pair("GET", 0));//!
	//method.insert(std::make_pair("HEAD", 0));
	accepted_methods.insert(std::make_pair("POST", 0));//!
	//method.insert(std::make_pair("PUT", 0));
	accepted_methods.insert(std::make_pair("DELETE", 0));//!
	//method.insert(std::make_pair("CONNECT", 0));
	///method.insert(std::make_pair("OPTIONS", 0));
	//method.insert(std::make_pair("TRACE", 0));
	//method.insert(std::make_pair("PATCH", 0));
}
Location::~Location()
{}

Location::Location(Location const &copy){*this = copy;}
Location &Location::operator=(Location const &eq)
{
	locationMask = eq.locationMask;
	accepted_methods = eq.accepted_methods;
	root = eq.root;
	alias = eq.alias;
	autoindex = eq.autoindex;
	route_for_uploaded_files = eq.route_for_uploaded_files;
	redirection_status_code = eq.redirection_status_code;//
	redirection_url = eq.redirection_url;
	file_request_if_dir = eq.file_request_if_dir;
	filename_cgi = eq.filename_cgi;

	return (*this);
}
		

void	Location::setRootUploadedFiles(list<string>::iterator &itList, std::list<std::string> tokenList)
{
	itList++;

	if ((*itList).empty() || (*itList).compare(0, 2, ";") == 0 || (*itList).compare(0, 2, "}") == 0 \
	|| (*itList).compare(0, 2, "{") == 0)
		throw "something wrong after alias";
	route_for_uploaded_files = *itList;
	//std::cout << "route_for_uploaded_files = " << route_for_uploaded_files << std::endl;
	itList++;
	if ((*itList).compare(0, 2, ";") != 0)
		throw "there is not ; after alias";
}

void	Location::setIndex(list<string>::iterator &itList, std::list<std::string> tokenList)
{
	itList++;

	if ((*itList).empty() || (*itList).compare(0, 2, ";") == 0)
		throw "something wrong after index";
	//while (itList != tokenList.end() && (*itList).compare(0, 2, ";") != 0)
	//{
		//file_request_if_dir.push_back(*itList);
		file_request_if_dir = *itList;
	//	std::cout << "file_request_if_dir = " << *itList << std::endl;
		itList++;
	//}
	if ((*itList).compare(0, 2, ";") != 0)
		throw "there is not ; after index";
}

void	Location::setRedirection(list<string>::iterator &itList, std::list<std::string> tokenList)
{
	size_t		i;
	string		return_status_code;
	std::stringstream buf;

	i = 0;
	itList++;
	if (itList != tokenList.end() && *itList != ";")
	{
		return_status_code = *itList;
		//std::cout << "return ==== " << *itList << std::endl;
		while (return_status_code[i])
		{
			if (return_status_code[i] < '0' || return_status_code[i] > '9')
				throw "there is not number in the return line before .";
			i++;
		}
		itList++;
		buf << return_status_code;
		buf >> redirection_status_code;
	}
	else
		throw "there is no argument in the return line";
	
	if (itList == tokenList.end())
		throw "there is no url in the return line";

	redirection_url = *itList;

	itList++;
	if (itList == tokenList.end() || *itList != ";")
		throw "there is no ; in the return line";
	

}

void	Location::setFileNameCGI(list<string>::iterator &itList, std::list<std::string> tokenList)
{
	itList++;

	if ((*itList).empty() || (*itList).compare(0, 2, ";") == 0)
		throw "something wrong after cgi_filename";

	filename_cgi = *itList;
	
	itList++;

	if ((*itList).compare(0, 2, ";") != 0)
		throw "there is not ; after cgi_filename";
}

void	Location::fillAll(list<string>::iterator &itList, list<std::string> tokenList)
{
	map<string, void (Location::*)(list<string>::iterator &itList, list<std::string> tokenList)> funMap;
	map<string, void (Location::*)(list<string>::iterator &itList, list<std::string> tokenList)>::iterator it;

	funMap.insert(std::make_pair("autoindex", &Location::setAutoindex));
	funMap.insert(std::make_pair("root", &Location::setRoot));
	funMap.insert(std::make_pair("alias", &Location::setAlias));
	funMap.insert(std::make_pair("method", &Location::setMethod));
	funMap.insert(std::make_pair("upload_store", &Location::setRootUploadedFiles));
	funMap.insert(std::make_pair("return", &Location::setRedirection));
	funMap.insert(std::make_pair("index", &Location::setIndex));
	funMap.insert(std::make_pair("cgi_filename", &Location::setFileNameCGI));

	itList++;

	locationMask = *itList;
	//std::cout << "*itList = " << *itList << std::endl;
	if (locationMask.empty() ||  locationMask == "{" \
		|| locationMask == "}" || locationMask == ";")
		throw "location there is not mask location";
	itList++;
	//std::cout << "locationMask = " << locationMask << std::endl;
	if ((*itList).empty() || *itList != "{")
		throw "location there is not {";
	itList++;
	while (itList != tokenList.end() && *itList != "{" && *itList != "}")
	{
		it = funMap.find(*itList);
		if (it != funMap.end())
			(this->*(it->second))(itList, tokenList);
		else
			throw "unknown directive";
		itList++;
	}
	if ((*itList).empty() || *itList != "}")
		throw "location there is not }";
}

void	Location::setAutoindex(list<string>::iterator &itList, list<std::string> tokenList)
{
	//itList != tokenList.end()
	itList++;

	if ((*itList).compare(0, 3, "on") != 0 && (*itList).compare(0, 4, "off"))
		throw "something wrong after autoindex";
	autoindex = *itList == "on" ? 1 : 0;
	itList++;
	if ((*itList).compare(0, 2, ";") != 0)
		throw "there is not ; after autoindex";
}

void	Location::setRoot(list<string>::iterator &itList, list<std::string> tokenList)
{
	itList++;

	if ((*itList).empty() || (*itList).compare(0, 2, ";") == 0 || (*itList).compare(0, 2, "}") == 0 \
	|| (*itList).compare(0, 2, "{") == 0)
		throw "something wrong after root";
	root = *itList;
	itList++;
	if ((*itList).compare(0, 2, ";") != 0)
		throw "there is not ; after root";

	//check root if is directory
}



void	Location::setAlias(list<string>::iterator &itList, list<std::string> tokenList)
{
	//itList != tokenList.end()
	itList++;

	if ((*itList).empty() || (*itList).compare(0, 2, ";") == 0 || (*itList).compare(0, 2, "}") == 0 \
	|| (*itList).compare(0, 2, "{") == 0)
		throw "something wrong after alias";
	alias = *itList;
	//std::cout << "alias = " << alias << std::endl;
	itList++;
	if ((*itList).compare(0, 2, ";") != 0)
		throw "there is not ; after alias";


	//check root if is directory
}

void	Location::setMethod(list<string>::iterator &itList, list<std::string> tokenList)
{
	map<string, int>::iterator it;
	itList++;

	if (itList == tokenList.end() || (*itList).empty() || \
		(*itList).compare(0, 2, ";") == 0 || (*itList).compare(0, 2, "}") == 0 \
		|| (*itList).compare(0, 2, "{") == 0)
		throw "something wrong after method";
	it = accepted_methods.find(*itList);
	if (it == accepted_methods.end())
		throw "there are not this method";
	it->second = 1;
	itList++;
	if ((*itList).compare(0, 2, ";") != 0)
		throw "there is not ; after method";
}

}