/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebservConf.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelisabe <nelisabe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/18 23:24:41 by bbenny            #+#    #+#             */
/*   Updated: 2021/07/16 15:44:22 by nelisabe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebservConf.hpp"
#include <fstream>
#include <iostream>
#include <stack>
#include <sstream>
#include <cctype>
//#include "url.hpp"

namespace config{
WebserverConf::WebserverConf(char const *name)
{
	this->readConfFile(name);
}
WebserverConf::~WebserverConf()
{
	set<tServer *>::iterator it;
	//for (it = pointerToServerSet.begin(); it != pointerToServerSet.end(); it++)
	//	delete *it;

}
//WebserverConf::WebserverConf(WebserverConf const &copy){}
//WebserverConf &WebserverConf::operator=(WebserverConf const &eq){}

void	tabToSpace(std::string &buf)
{
	size_t found;

	found = buf.find('	');
	while (found != std::string::npos)
	{
		buf.replace(found, 1, " ");
		found = buf.find('	');
	}
}

void	deleteAllAfterSharp(std::string &buf)
{
	size_t	found;

	found = buf.find("#");
	if (found != std::string::npos)
		buf.erase(found);
}

void	deleteSpaces(std::string &buf)
{
	size_t	found;
	//size_t	foundEndStr;

	tabToSpace(buf);

	while (buf[0] && buf[0] == ' ')
		buf.erase(0, 1);

	found = buf.find("  ");
	while (found != std::string::npos)
	{
		buf.replace(found, 2, " ");
		found = buf.find("  ");
	}

	if (buf.size() != 0 && buf[buf.size() - 1] == ' ')
		buf.erase(buf.size() - 1);
	
	/*found = buf.find("; ");
	if (found != std::string::npos)
		buf.replace(found, 2 ,";");*/
}

void	checkLine(std::string const &buf)
{
	size_t found;

	if (buf.size() != 0 && (buf[buf.size() - 1] != ';' \
	&& buf[buf.size() - 1] != '{' && buf[buf.size() - 1] != '}'))
	{
		std::cout << "End of line syntax error" << std::endl;
		throw "syntax error";
	}

	if (buf.size() \
		&& (found = buf.find(';')) != std::string::npos \
		&& (found = buf.find(';')) != buf.size() - 1)
	{
		std::cout << "there is ; not only the End of line syntax error" << std::endl;
		throw "syntax error";
	}

	if (buf.size() \
		&& (found = buf.find('{')) != std::string::npos \
		&& (found = buf.find('{')) != buf.size() - 1)
	{
		std::cout << "there is { not only the End of line syntax error" << std::endl;
		throw "syntax error";
	}

	if (buf.size() \
		&& (found = buf.find('}')) != std::string::npos \
		&& (found = buf.find('}')) != buf.size() - 1)
	{
		std::cout << "there is } not only the End of line syntax error" << std::endl;
		throw "syntax error";
	}
}

void	WebserverConf::fileToListLine(const char *confFileName)
{
	std::ifstream	ifs;
	std::string		buf;

	ifs.open(confFileName,  std::ifstream::in);
	if (!ifs.is_open())
	{
		std::cout << "Configuration file cannot be opened" << std::endl;//throw
		throw "error opening the configuration file";
	}

	std::getline(ifs, buf);
	while(ifs)
	{
		deleteAllAfterSharp(buf);
		deleteSpaces(buf);//delete all space
		checkLine(buf);//check ; or { } in the end of line
		if (buf.size())
		{
			this->fileLineToList.push_back(buf);
		}
		buf.clear();
		std::getline(ifs, buf);
	}
}

void	WebserverConf::allLinesToToken()
{
	size_t		i;
	char		*str;
	std::string	buf;

	
	std::list<std::string>::iterator it;
	std::list<std::string>::iterator itToken;
	for(it = fileLineToList.begin(); it != fileLineToList.end(); it++)
	{
		str = (char *)(*it).c_str();
		i = 0;
		while (str && str[i])
		{
			while (str[i] && str[i] != ' ' && str[i] != ';' \
				&& str[i] != '{' && str[i] != '}')
			{
				buf += str[i++];
			}
			if (buf.size())
			{
				tokenList.push_back(buf);
				buf.clear();
			}
			if (str[i] && str[i] != ' ')
			{
				buf.clear();
				buf += str[i];
				tokenList.push_back(buf);
			}
			buf.clear();
			if (str[i])
				i++;
		}
	}
}

void	WebserverConf::checkToken()
{
	int									countCurlyBracket;
	std::list<std::string>::iterator	itList;

	countCurlyBracket = 0;
	itList = tokenList.begin();
	while (itList != tokenList.end())
	{
		if (*itList != "server")
			throw "Error: not only server";
		itList++;
		if (*itList != "{")
			throw "Error: not curly brackets after server";
		while (itList != tokenList.end() && !(*itList == "}" && countCurlyBracket == 1))
		{
			if (*itList == "{")
				countCurlyBracket++;
			else if (*itList == "}")
				countCurlyBracket--;
			if (countCurlyBracket < 0)
				throw "Error curly brackets";
			itList++;
		}
		if (itList != tokenList.end() && *itList == "}")
		{
			countCurlyBracket--;
			itList++;
		}
	}
	if (countCurlyBracket != 0)
		throw "Error curly brackets";
}

/*void	checkIp(std::string &ipString)
{
	std::list<std::string> tokenIpString;
}*/

void	WebserverConf::setListen(std::list<std::string>::iterator &itList, tServer	&server,std::map<int, std::string> &map_ip_port)
{
	std::size_t	i;
	std::size_t	iDot;
	std::size_t	iDoubleDot;
	std::size_t	countDot = 0;
	int			num = 0;
	std::string	ip;
	int	port = 0;

	itList++;
	server.listen = *itList;

	itList++;
	if (*itList != ";")
		throw "there is not ; in the listen line after one param";

	i = server.listen.find("localhost");
	if (i != std::string::npos)
		server.listen.replace(i, 9, "127.0.0.1");
	
	i = 0;
	iDoubleDot = server.listen.find(":");
	if (iDoubleDot != std::string::npos)
	{
		i = 0;
		iDot = server.listen.find(".");
		if (iDot != std::string::npos)
			countDot++;
		num = 0;
		while (iDot != std::string::npos && i < iDoubleDot)
		{
			num = 0;
			if (i >= iDot)
				throw "there is a problem - there is not number между dots in the listen line";
			while (i < iDot)
			{
				if (server.listen[i] < '0' || server.listen[i] > '9')
					throw "there is not number in the listen line before .";
				num = num * 10 + server.listen[i] - '0';
				i++;
			}
			if (num > 255)
				throw "there is a big number in the listen line";
			i++;
			
			iDot = server.listen.find(".", iDot + 1);
			if (iDot == std::string::npos)
				iDot = iDoubleDot;
			else
				countDot++;
			
		}
		//check ip if correct
		if (countDot != 3)
			throw "there are not 4 number in ip adress in the listen line";
		ip = server.listen.substr(0, iDoubleDot);
	}
	else
		//ip = "default";
		throw "there are not ip adress in the listen line";
	while (server.listen[i])
	{
		if (server.listen[i] < '0' || server.listen[i] > '9')
			throw "there is not number in the listen line before ;";
		port = port * 10 + server.listen[i] - '0';
		i++;
	}
	itList++;
	if (map_ip_port.count(port) != 0)
		throw "this port has already done in this server ";
	map_ip_port[port] = ip;
	//server.port = port;//map::<port, ip> if count != 0 error
	//server.ip = ip;//== if no ==>
	//std::cout << "ip = " << server.ip << std::endl;
}

void	WebserverConf::setServer_name(std::list<std::string>::iterator &itList, tServer	&server,std::map<int, std::string>&)
{
	itList++;
	while (itList != tokenList.end() && *itList != ";" && *itList != "{" && *itList != "}")
	{
		//std::cout << "all names " << *itList << std::endl;
		server.server_name.push_back(*itList);
		itList++;
	}

	if (itList == tokenList.end() || *itList != ";")
		throw "there is no ; in the Server_name line";
	itList++;
}

void	WebserverConf::setError_page(std::list<std::string>::iterator &itList, tServer	&server,std::map<int, std::string>&)
{
	int nError = 0;
	std::string file;
	std::string numString;
	size_t i = 0;
	std::stack<std::string> errorStack;

	itList++;
	while (itList != tokenList.end() && *itList != ";" && *itList != "{" && *itList != "}")
	{
		errorStack.push(*itList);
		itList++;
	}
	if (*itList != ";")
		throw "There is no ; in the error line";
	*itList++;
	if (errorStack.size() < 2)
		throw "There is not enough arguments in the error line";
	if (!errorStack.empty())
	{
		file = errorStack.top();
		errorStack.pop();
		std::cout << "file = " << file << std::endl;
		//try to open file
	}
	while (!errorStack.empty())
	{
		i = 0;
		nError = 0;
		numString = errorStack.top();
		errorStack.pop();
		while (numString[i])
		{
			if (numString[i] < '0' || numString[i] > '9')
				throw "there is not number in the errror num line before .";
			nError = nError * 10 + numString[i] - '0';
			i++;
		}
		if (server.error_page.find(nError) == server.error_page.end())
			server.error_page.insert(make_pair(nError, file));
	}
	map<int, string>::iterator it = server.error_page.begin();
	while (it != server.error_page.end())
	{
		std::cout << it->first << " " << it->second << std::endl;
		it++;
	}
}

void	WebserverConf::setClient_max_body_size(std::list<std::string>::iterator &itList, tServer &server,std::map<int, std::string>&)
{
	size_t		i;
	string		client_max_body_size;
	std::stringstream buf;

	i = 0;
	itList++;
	if (itList != tokenList.end() && *itList != ";" && *itList != "{" && *itList != "}")
	{
		client_max_body_size = *itList;
		std::cout << "client_max_body_size ==== " << *itList << std::endl;
		while (client_max_body_size[i])
		{
			if (client_max_body_size[i] < '0' || client_max_body_size[i] > '9')
				throw "there is not number in the Client_max_body_size line before .";
			i++;
		}
		itList++;
		buf << client_max_body_size;
		buf >> server.client_max_body_size;
	}
	else
		throw "there is no argument in the Client_max_body_size line";
	if (itList == tokenList.end() || *itList != ";")
		throw "there is no ; in the Server_name line";	
	itList++;

}

void	WebserverConf::setLocation(std::list<std::string>::iterator &itList, tServer &server,std::map<int, std::string>&)
{
	Location loc;// = new Location ;
	
	loc.fillAll(itList, tokenList);
	server.locationMap.push_back(loc);
	
}

void	WebserverConf::fillOneServer(std::list<std::string>::iterator itList)
{
	std::map<int, std::string> map_port_ip;//
	tServer	server;// = new tServer;
	size_t		i;
	std::array<std::string, 5> arrVar = {{"listen", "server_name", \
	"error_page", "client_max_body_size", "location"}};
	void	(WebserverConf::*arrF[])(std::list<std::string>::iterator &itList, tServer	&server,std::map<int, std::string> &map_ip_port) = \
	{&WebserverConf::setListen, &WebserverConf::setServer_name, \
	&WebserverConf::setError_page, &WebserverConf::setClient_max_body_size, \
	&WebserverConf::setLocation};
	//int		port;

	if (itList != tokenList.end() && *itList == "{")
		itList++;
	else
		throw "some error with configfile";
	while (itList != tokenList.end() && (*itList).compare(0, 7, "server") != 0)
	{
		i = 0;
		
		while (i < arrVar.size())
		{
			if (arrVar[i].compare(*itList) == 0)
			{
				(this->*arrF[i])(itList, server, map_port_ip);
				break ;
			}
			i++;
		}
		if (i == arrVar.size())
			throw "unknown directive";
		if ((*itList).compare(0, 2, "}") == 0)
			break ;
	}
	if ((*itList).compare(0, 2, "}") != 0)
		throw "there is not }";
	fillMapServer(server, map_port_ip);//измениьь прототип +map
}

void	WebserverConf::fillMapServer(tServer &server, std::map<int, std::string> &map_port_ip)
{
	
	//	tServer	*	server = new tServer(tserver);
	//server.ip = map[0] map <port, ip>
	//server.port =

//	pointerToServerSet.insert(server);

	//map<int, map<string, map<string, tServer *> > >::iterator it;
	map<int, map<string, tServer > >::iterator it;
	map<string, tServer >  tmp;
	//map<string, map<string, tServer *> > tmpMap;
	
	std::list<std::string>::iterator itLst;
	//itLst = server.server_name.begin();

std::map<int, std::string>::iterator itMpPortIP = map_port_ip.begin();

while (itMpPortIP != map_port_ip.end())
{
	itMpPortIP++;
	if (map_global_port_ip.count(itMpPortIP->first))//if there is the port
		if (map_global_port_ip[itMpPortIP->first] != map_port_ip[itMpPortIP->first]) //ip !the same
			return ;
}
itMpPortIP = map_port_ip.begin();
while (itMpPortIP != map_port_ip.end())
{
	if (serverMap.count(itMpPortIP->first))
	{
		itLst = (server.server_name).begin();
		while (itLst != server.server_name.end())
		{
			if (serverMap[itMpPortIP->first].count(*itLst) == 0)//server_name был- не добавляем
			//иначе добавляем
			{
				serverMap[itMpPortIP->first][*itLst] = server;
				itLst++;
			}
		}
	}
	else
	{
		map_global_port_ip[itMpPortIP->first] = itMpPortIP->second;
		serverMap[itMpPortIP->first]["default"] = server;
		itLst = server.server_name.begin();
		while (itLst != server.server_name.end())
		{
			serverMap[itMpPortIP->first][*itLst] = server;
			itLst++;
		}
	
	}
	itMpPortIP++;
}

/*	it = serverMap.find(server->port);
	if (it != serverMap.end())
	{
		//check ip
	//	if (serverMap[server->port].begin()->second->ip != server->ip)
	//		throw "different ip with the same port";
		itLst = server->server_name.begin();
		while (itLst != server->server_name.end())
		{
			tmp.insert(std::make_pair(*itLst, server));
			itLst++;
		}
	}
	else
	{
		itLst = server->server_name.begin();
		while (itLst != server->server_name.end())
		{
			tmp.insert(std::make_pair(*itLst, server));
			itLst++;
		}
		tmp.insert(std::make_pair("default", server));
		serverMap.insert(std::make_pair(server->port, tmp));
		itLst++;
	}*/
}

void	WebserverConf::tokenToServerMap()
{
	std::list<std::string>::iterator	itList;

	itList = tokenList.begin();
	while (itList != tokenList.end())
	{
		itList++;
		fillOneServer(itList);

		while (itList != tokenList.end() && (*itList).compare(0, 7, "server") != 0)
			itList++;
	}
}

void	WebserverConf::readConfFile(const char *confFileName)
{
	//std::string		buf;//= NULL;

	fileToListLine(confFileName);
	allLinesToToken();//read by one symbol
	checkToken();
	tokenToServerMap();

	//printServerMap();
}

map<int, map<string, tServer > >  const &WebserverConf::getServerMap() const {return serverMap;}
std::map<int, std::string> const &WebserverConf::getPorts() const { return map_global_port_ip; }
/*
bool is_octet(const std::string &str) {
  if (str.length() == 1) {
	if (std::isdigit(str[0]))
	  return true;
  } else if (str.length() == 2) {
	return true;
  } else if (str.length() == 3) {
	if (str[0] == '1') {
	  if (std::isdigit(str[1]) && std::isdigit(str[2]))
		return true;
	} else if (str[0] == '2') {
	  if (str[1] == '0' || str[1] == '1' || str[1] == '2' || str[1] == '3' || str[1] ==
		  '4') {
		if (std::isdigit(str[2]))
		  return true;
	  } else if (str[1] == '5') {
		if (str[2] == '0' || str[2] == '1' || str[2] == '2' || str[2] == '3' || str[2]
			== '4' || str[2] == '5')
		  return true;
	  }
	}
  }
  return false;
}

bool isIPv4(const std::string &str) {
  size_t pos = 0;
  for (int i = 0; i < 4; ++i) {
	size_t end = str.find('.', pos);
	if (i != 3 && end == std::string::npos)
	  return false;
	if (!is_octet(str.substr(pos, end - pos)))
	  return false;
	pos = ++end;
  }
  return true;
}*/

tServerInformation	chooseServer(http::url::URL url, map<string, tServer >  tmp)
{
	tServerInformation serverInformation;

	//serverInformation.error_pages.insert(make_pair(0, "default_error_page"));//0 default
	serverInformation.limit_size = 100000;//-1?64
	serverInformation.autoindex = 0;//0
	//serverInformation.file_request_if_dir = "file_request_if_dir";//default
	serverInformation.redirection_status_code = 0;//
	serverInformation.redirection_url = "";//
	serverInformation.name_file = "name_file";//url_after_alias!
//	serverInformation.accepted_methods.insert(std::make_pair("GET", 1));//map->set
//	serverInformation.accepted_methods.insert(std::make_pair("DELETE", 1));
//	serverInformation.accepted_methods.insert(std::make_pair("POST", 1));
	serverInformation.route_for_uploaded_files = "route_for_uploaded_files";

	
/*	int port = 80;
	string server_name = "default";
//	string ip = "default";
	std::stringstream buf;
	std::size_t found = (url.host).find_last_of(":");
	if (found != std::string::npos)
	{
		buf << url.host.substr(found+1);
		buf >> port;//need erase
		url.host.erase(url.host.begin() + found, url.host.end());
	}
	if (http::url::isIPv4(url.host))
		ip = url.host;//server_name =default
	else
		server_name = url.host;
	map<int, map<string, tServer *> > ::const_iterator itMap = serverMap.find(port);*/
	string server_name;
	std::stringstream buf;
	int port;
	std::size_t found = (url.host).find_last_of(":");
	if (found != std::string::npos)
	{
		buf << url.host.substr(found+1);
		buf >> port;//need erase
		url.host.erase(url.host.begin() + found, url.host.end());
	}
	if (http::url::isIPv4(url.host))
		server_name = "default";
	else
		server_name = url.host;
	//if (itMap == serverMap.end() && ip == "default")

	//map<string, tServer > ::const_iterator itMap = tmp.begin();
	//if (itMap != tmp.end())
	//{
		map<string, tServer > ipServer = tmp;
		std::cout << "server_name :" << server_name << std::endl;
		map<string, tServer > ::iterator itIpSrv = ipServer.find(server_name);
		if (itIpSrv != ipServer.end())
		{
			//map<string, tServer > nameServer = ipServer;
			map<string, tServer >::iterator itNameSrv = itIpSrv;//nameServer.find(server_name);
			//if (itNameSrv == nameServer.end())//??
			//	itNameSrv = nameServer.find("default");
			tServer server = itNameSrv->second;

			list<string>::iterator itList = server.server_name.begin();
			serverInformation.limit_size = server.client_max_body_size;
			map<int, string>::iterator itEr = server.error_page.begin();
//				if (itEr == server.error_page.end())
//					server.error_page.insert(make_pair(0, "error_page_default"));
			serverInformation.error_pages = server.error_page;
			vector<Location>::iterator itLoc = server.locationMap.begin();
			string locationMask;
			string path_for_alias = url.path;
			while (itLoc != server.locationMap.end() && path_for_alias != "")//search location by find
			{
				
				while (itLoc != server.locationMap.end() && path_for_alias != "")
				{
					if (itLoc->locationMask == path_for_alias)
					{
						serverInformation.name_file = url.path;
						serverInformation.name_file.erase(0, path_for_alias.length());//need to insert
					
						serverInformation.name_file = itLoc->alias + serverInformation.name_file;//need to clear
					

						serverInformation.accepted_methods = itLoc->accepted_methods;
					
						serverInformation.redirection_status_code = itLoc->redirection_status_code;
						serverInformation.redirection_url = itLoc->redirection_url;

						serverInformation.file_request_if_dir = itLoc->file_request_if_dir;

						itLoc = server.locationMap.end();
						path_for_alias = "";

		
						break;
					}
					
					
					itLoc++;
				}
				if (itLoc == server.locationMap.end() && path_for_alias != "")
				{
					if (path_for_alias.find("/") != std::string::npos)
					{
						if (path_for_alias.find_last_of("/") == path_for_alias.length() - 1)
							path_for_alias = path_for_alias.erase(path_for_alias.find_last_of("/"));
						else
							path_for_alias = path_for_alias.erase(path_for_alias.find_last_of("/") + 1);
					}
					else
						path_for_alias = path_for_alias.erase();
					
					//std::cout << "path_for_alias " << path_for_alias << std::endl;
				
				}
				itLoc = server.locationMap.begin();
			}
		}
	//}
	return serverInformation;
}
}