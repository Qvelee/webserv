/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebservConf.cpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelisabe <nelisabe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/18 23:24:41 by bbenny            #+#    #+#             */
/*   Updated: 2021/07/15 14:03:20 by nelisabe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "WebservConf.hpp"
#include <fstream>
#include <iostream>
#include <stack>
#include <sstream>
#include <cctype>
#include <unistd.h>

namespace config {
WebserverConf::WebserverConf(char const *name) {
  this->readConfFile(name);
}
WebserverConf::~WebserverConf() {
  set<tServer *>::iterator it;

}
//WebserverConf::WebserverConf(WebserverConf const &copy){}
//WebserverConf &WebserverConf::operator=(WebserverConf const &eq){}

void tabToSpace(std::string &buf) {
  size_t found;

  found = buf.find('	');
  while (found != std::string::npos) {
	buf.replace(found, 1, " ");
	found = buf.find('	');
  }
}

void deleteAllAfterSharp(std::string &buf) {
  size_t found;

  found = buf.find('#');
  if (found != std::string::npos)
	buf.erase(found);
}

void deleteSpaces(std::string &buf) {
  size_t found;

  tabToSpace(buf);

  while (buf[0] && buf[0] == ' ')
	buf.erase(0, 1);

  found = buf.find("  ");
  while (found != std::string::npos) {
	buf.replace(found, 2, " ");
	found = buf.find("  ");
  }

  if (buf.size() != 0 && buf[buf.size() - 1] == ' ')
	buf.erase(buf.size() - 1);
}

void checkLine(std::string const &buf, size_t num_line) {
  size_t found;

  if (buf.size() != 0 && (buf[buf.size() - 1] != ';' \
 && buf[buf.size() - 1] != '{' && buf[buf.size() - 1] != '}')) {
	std::cout << "End of line syntax error in the line " << num_line << std::endl;
	throw "syntax error";
  }

  if (buf.size() \
 && (found = buf.find(';')) != std::string::npos \
 && (found = buf.find(';')) != buf.size() - 1) {
	std::cout << "there is ; not only the End of line syntax error in the line "
			  << num_line << std::endl;
	throw "syntax error";
  }

  if (buf.size() \
 && (found = buf.find('{')) != std::string::npos \
 && (found = buf.find('{')) != buf.size() - 1) {
	std::cout << "there is { not only the End of line syntax error in the line "
			  << num_line << std::endl;
	throw "syntax error";
  }

  if (buf.size() \
 && (found = buf.find('}')) != std::string::npos \
 && (found = buf.find('}')) != buf.size() - 1) {
	std::cout << "there is } not only the End of line syntax error in the line "
			  << num_line << std::endl;
	throw "syntax error";
  }
}

void WebserverConf::fileToListLine(const char *confFileName) {
  std::ifstream ifs;
  std::string buf;

  ifs.open(confFileName, std::ifstream::in);
  if (!ifs.is_open()) {
	std::cout << "Configuration file cannot be opened" << std::endl;//throw
	throw "error opening the configuration file";
  }

  std::getline(ifs, buf);
  size_t i = 1;
  while (ifs) {
	deleteAllAfterSharp(buf);
	deleteSpaces(buf);//delete all space
	checkLine(buf, i++);//check ; or { } in the end of line
	if (buf.size()) {
	  this->fileLineToList.push_back(buf);
	}
	buf.clear();
	std::getline(ifs, buf);
  }
}

void WebserverConf::allLinesToToken() {
  size_t i;
  char *str;
  std::string buf;

  std::list<std::string>::iterator it;
  std::list<std::string>::iterator itToken;
  for (it = fileLineToList.begin(); it != fileLineToList.end(); it++) {
	str = (char *) (*it).c_str();
	i = 0;
	while (str && str[i]) {
	  while (str[i] && str[i] != ' ' && str[i] != ';' \
 && str[i] != '{' && str[i] != '}') {
		buf += str[i++];
	  }
	  if (buf.size()) {
		tokenList.push_back(buf);
		buf.clear();
	  }
	  if (str[i] && str[i] != ' ') {
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

void WebserverConf::checkToken() {
  int countCurlyBracket;
  std::list<std::string>::iterator itList;

  countCurlyBracket = 0;
  itList = tokenList.begin();
  while (itList != tokenList.end()) {
	if (*itList != "server")
	  throw "Error: not only server";
	itList++;
	if (*itList != "{")
	  throw "Error: not curly brackets after server";
	while (itList != tokenList.end() && !(*itList == "}" && countCurlyBracket == 1)) {
	  if (*itList == "{")
		countCurlyBracket++;
	  else if (*itList == "}")
		countCurlyBracket--;
	  if (countCurlyBracket < 0)
		throw "Error curly brackets";
	  itList++;
	}
	if (itList != tokenList.end() && *itList == "}") {
	  countCurlyBracket--;
	  itList++;
	}
  }
  if (countCurlyBracket != 0)
	throw "Error curly brackets";
}

void WebserverConf::setListen(std::list<std::string>::iterator &itList,
							  tServer &server,
							  std::map<int, std::string> &map_ip_port) {
  std::size_t i;
  std::size_t iDot;
  std::size_t iDoubleDot;
  std::size_t countDot = 0;
  int num = 0;
  std::string ip;
  int port = 0;

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
  if (iDoubleDot != std::string::npos) {
	i = 0;
	iDot = server.listen.find(".");
	if (iDot != std::string::npos)
	  countDot++;
	num = 0;
	while (iDot != std::string::npos && i < iDoubleDot) {
	  num = 0;
	  if (i >= iDot)
		throw "there is a problem - there is not number между dots in the listen line";
	  while (i < iDot) {
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
  } else
	throw "there are not ip adress in the listen line";
  while (server.listen[i]) {
	if (server.listen[i] < '0' || server.listen[i] > '9')
	  throw "there is not number in the listen line before ;";
	port = port * 10 + server.listen[i] - '0';
	i++;
  }
  itList++;
  if (map_ip_port.count(port) != 0)
	throw "this port has already done in this server ";
  map_ip_port[port] = ip;
}

void WebserverConf::setServer_name(std::list<std::string>::iterator &itList,
								   tServer &server,
								   std::map<int, std::string> &) {
  itList++;
  while (itList != tokenList.end() && *itList != ";" && *itList != "{" && *itList != "}") {
	server.server_name.push_back(*itList);
	itList++;
  }

  if (itList == tokenList.end() || *itList != ";")
	throw "there is no ; in the Server_name line";
  itList++;
}

void WebserverConf::setError_page(std::list<std::string>::iterator &itList,
								  tServer &server,
								  std::map<int, std::string> &) {
  int nError = 0;
  std::string file;
  std::string numString;
  size_t i = 0;
  std::stack<std::string> errorStack;

  itList++;
  while (itList != tokenList.end() && *itList != ";" && *itList != "{" && *itList != "}") {
	errorStack.push(*itList);
	itList++;
  }
  if (*itList != ";")
	throw "There is no ; in the error line";
  *itList++;
  if (errorStack.size() < 2)
	throw "There is not enough arguments in the error line";
  if (!errorStack.empty()) {
	file = errorStack.top();
	errorStack.pop();
  }
  while (!errorStack.empty()) {
	i = 0;
	nError = 0;
	numString = errorStack.top();
	errorStack.pop();
	while (numString[i]) {
	  if (numString[i] < '0' || numString[i] > '9')
		throw "there is not number in the errror num line before .";
	  nError = nError * 10 + numString[i] - '0';
	  i++;
	}
	if (server.error_page.find(nError) == server.error_page.end())
	  server.error_page.insert(make_pair(nError, file));
  }
}

void WebserverConf::setClient_max_body_size(std::list<std::string>::iterator &itList,
											tServer &server,
											std::map<int, std::string> &) {
  size_t i;
  string client_max_body_size;
  std::stringstream buf;

  i = 0;
  itList++;
  if (itList != tokenList.end() && *itList != ";" && *itList != "{" && *itList != "}") {
	client_max_body_size = *itList;

	while (client_max_body_size[i]) {
	  if (client_max_body_size[i] < '0' || client_max_body_size[i] > '9')
		throw "there is not number in the Client_max_body_size line before .";
	  i++;
	}
	itList++;
	buf << client_max_body_size;
	buf >> server.client_max_body_size;
  } else
	throw "there is no argument in the Client_max_body_size line";
  if (itList == tokenList.end() || *itList != ";")
	throw "there is no ; in the Server_name line";
  itList++;

}

void WebserverConf::setLocation(std::list<string>::iterator &itList,
								tServer &server,
								map<int, string> &) {
  Location loc;// = new Location ;

  loc.fillAll(itList, tokenList);
  server.locationMap.push_back(loc);

}

void WebserverConf::setCgi(std::list<string>::iterator &itList,
						   tServer &server,
						   map<int, string> &) {
  string key;

  itList++;
  if (itList != tokenList.end() && *itList != ";" && *itList != "{" && *itList != "}") {
	key = *itList;
	itList++;
  } else
	throw "there are not enough elements after cgi ";

  if (itList != tokenList.end() && *itList != ";" && *itList != "{" && *itList != "}") {
	server.cgi_handler.insert(std::make_pair(key, *itList));
	itList++;
  } else
	throw "there are not enough elements after cgi ";

  if (itList == tokenList.end() || *itList != ";")
	throw "there is no ; in the Server_name line";
  itList++;
}

void WebserverConf::fillOneServer(std::list<std::string>::iterator itList) {
  std::map<int, std::string> map_port_ip;//
  tServer server;// = new tServer;
  server.client_max_body_size = 100000;
  size_t i;
  std::vector<std::string> arrVar;
  arrVar.push_back("listen");
  arrVar.push_back("server_name");
  arrVar.push_back("error_page");
  arrVar.push_back("client_max_body_size");
  arrVar.push_back("location");
  arrVar.push_back("cgi");
  void (WebserverConf::*arrF[])(std::list<std::string>::iterator &itList, \
                tServer &server, map<int, string> &map_ip_port) = \
        {&WebserverConf::setListen, &WebserverConf::setServer_name, \
        &WebserverConf::setError_page, &WebserverConf::setClient_max_body_size, \
        &WebserverConf::setLocation, &WebserverConf::setCgi};
  //int		port;

  if (itList != tokenList.end() && *itList == "{")
	itList++;
  else
	throw "some error with configfile";
  while (itList != tokenList.end() && (*itList).compare(0, 7, "server") != 0) {
	i = 0;

	while (i < arrVar.size()) {
	  if (arrVar[i].compare(*itList) == 0) {
		(this->*arrF[i])(itList, server, map_port_ip);
		break;
	  }
	  i++;
	}
	if (i == arrVar.size())
	  throw "unknown directive";
	if ((*itList).compare(0, 2, "}") == 0)
	  break;
  }
  if ((*itList).compare(0, 2, "}") != 0)
	throw "there is not }";
  fillMapServer(server, map_port_ip);//измениьь прототип +map
}

void WebserverConf::fillMapServer(tServer &server, map<int, string> &map_port_ip) {
  map<int, map<string, tServer> >::iterator it;
  map<string, tServer> tmp;

  std::list<std::string>::iterator itLst;

  std::map<int, std::string>::iterator itMpPortIP = map_port_ip.begin();

  while (itMpPortIP != map_port_ip.end()) {
	itMpPortIP++;
	if (map_global_port_ip.count(itMpPortIP->first))//if there is the port
	  if (map_global_port_ip[itMpPortIP->first]
		  != map_port_ip[itMpPortIP->first]) //ip !the same
		return;
  }
  itMpPortIP = map_port_ip.begin();
  while (itMpPortIP != map_port_ip.end()) {
	if (serverMap.count(itMpPortIP->first)) {
	  itLst = (server.server_name).begin();
	  while (itLst != server.server_name.end()) {
		if (serverMap[itMpPortIP->first].count(*itLst) == 0)//server_name был- не добавляем
		  //иначе добавляем
		{
		  serverMap[itMpPortIP->first][*itLst] = server;
		  itLst++;
		}
	  }
	} else {
	  map_global_port_ip[itMpPortIP->first] = itMpPortIP->second;
	  serverMap[itMpPortIP->first]["default"] = server;
	  itLst = server.server_name.begin();
	  while (itLst != server.server_name.end()) {
		serverMap[itMpPortIP->first][*itLst] = server;
		itLst++;
	  }

	}
	itMpPortIP++;
  }
}

void WebserverConf::tokenToServerMap() {
  std::list<std::string>::iterator itList;

  itList = tokenList.begin();
  while (itList != tokenList.end()) {
	itList++;
	fillOneServer(itList);

	while (itList != tokenList.end() && (*itList).compare(0, 7, "server") != 0)
	  itList++;
  }
}

void WebserverConf::readConfFile(const char *confFileName) {

  fileToListLine(confFileName);
  allLinesToToken();//read by one symbol
  checkToken();
  tokenToServerMap();

}

map<int,
	map<string, tServer> > const &WebserverConf::getServerMap() const { return serverMap; }
map<int,
	string> const &WebserverConf::getPorts() const { return map_global_port_ip; }//getter


tServerInformation chooseServer(http::url::URL url, map<string, tServer> tmp) {
  tServerInformation serverInformation;

  //serverInformation.limit_size = 100000;
  serverInformation.autoindex = false;
  serverInformation.redirection_status_code = 0;
  serverInformation.is_cgi = false;

  string server_name;
  std::stringstream buf;
  std::string tmp_file_name;
  int port;
  std::size_t found = (url.host).find_last_of(":");
  if (found != std::string::npos) {
	buf << url.host.substr(found + 1);
	buf >> port;
	url.host.erase(url.host.begin() + found, url.host.end());
  }
  if (http::url::isIPv4(url.host) || url.host == "localhost")
	server_name = "default";
  else
	server_name = url.host;

  map<string, tServer> ipServer = tmp;
  map<string, tServer>::iterator itIpSrv = ipServer.find(server_name);
  if (itIpSrv != ipServer.end()) {
	//std::cout << "here \n";
	map<string, tServer>::iterator itNameSrv = itIpSrv;

	tServer server = itNameSrv->second;

	serverInformation.limit_size = server.client_max_body_size;

	serverInformation.error_pages = server.error_page;

	//CGI
	map<string, string>::iterator itCgiHandler = server.cgi_handler.begin();
	char cur_dir[256];
	int i = 0;
	while (i != 256) {
	  cur_dir[i] = 0;
	  i++;
	}
	getcwd(cur_dir, 256);

	while (itCgiHandler != server.cgi_handler.end())//if (itLoc->filename_cgi != "")
	{
	  string key = itCgiHandler->first;
	  size_t n = url.path.find(itCgiHandler->first);
	  if (n != std::string::npos) {
		serverInformation.is_cgi = true;
		serverInformation.cgi_handler = itCgiHandler->second;
		serverInformation.cgi["SCRIPT_NAME"] = url.path.substr(0, n + key.length());
		serverInformation.cgi["SCRIPT_FILENAME"] =
			cur_dir + url.path.substr(0, n + key.length());//
		serverInformation.cgi["QUERY_STRING"] = url.raw_query;
		serverInformation.cgi["PATH_INFO"] = url.path.substr(n + key.length());
		serverInformation.cgi["PATH_TRANSLATED"] = cur_dir + url.path.substr(n + key.length());
		//serverInformation.cgi["REQUEST_METHOD"] = ;
		//serverInformation.cgi["CONTENT_TYPE"] =;
		//serverInformation.cgi["CONTENT_LENGTH"] =;
		break;
	  }
	  itCgiHandler++;
	}

	vector<Location>::iterator itLoc = server.locationMap.begin();
	string locationMask;
	string path_for_alias = url.path;
	//std::cout << "path for alias "<< itLoc->alias << std::endl;
	while (itLoc != server.locationMap.end() && path_for_alias != "") {
	  while (itLoc != server.locationMap.end() && path_for_alias != "") {
		if (itLoc->locationMask == path_for_alias) {
		  serverInformation.name_file = url.path;
		  if (itLoc->alias != "") {
			serverInformation.name_file.erase(0, path_for_alias.length());

			//add

			tmp_file_name = itLoc->alias + serverInformation.name_file;
		  } else
			tmp_file_name = url.path;
		  if (/*tmp_file_name.size() > 1 && */tmp_file_name[0] == '/')
			tmp_file_name.erase(0, 1);
		  serverInformation.name_file = tmp_file_name;

		  serverInformation.accepted_methods = itLoc->accepted_methods;

		  serverInformation.redirection_status_code = itLoc->redirection_status_code;
		  serverInformation.redirection_url = itLoc->redirection_url;

		  serverInformation.autoindex = itLoc->autoindex;

		  serverInformation.file_request_if_dir = itLoc->file_request_if_dir;

		  serverInformation.route_for_uploaded_files = itLoc->route_for_uploaded_files;


		  //
		  itLoc = server.locationMap.end();
		  path_for_alias = "";

		  break;
		}

		itLoc++;
	  }
	  if (itLoc == server.locationMap.end() && path_for_alias != "") {
		if (path_for_alias.find("/") != std::string::npos) {
		  if (path_for_alias.find_last_of("/") == path_for_alias.length() - 1)
			path_for_alias = path_for_alias.erase(path_for_alias.find_last_of("/"));
		  else
			path_for_alias = path_for_alias.erase(path_for_alias.find_last_of("/") + 1);
		} else
		  path_for_alias = path_for_alias.erase();

	  }
	  itLoc = server.locationMap.begin();
	}
  }
  return serverInformation;
}
}