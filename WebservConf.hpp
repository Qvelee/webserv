/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebservConf.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbenny <bbenny@student.21-school.ru>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/18 23:24:54 by bbenny            #+#    #+#             */
/*   Updated: 2021/04/18 23:24:57 by bbenny           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERVCONF_HPP
#define WEBSERVCONF_HPP

#include <string>
#include <map>
#include <list>
#include <vector>
#include <array>
#include <iostream>
#include "Location.hpp"
#include <set>
#include "url.hpp"


namespace config{

using std::map;
using std::string;
using std::vector;
using std::list;
using std::set;
using std::make_pair;


///www.f.ru/i/rt/s - с конца отрезаем

///i/
///i/rt/

//need to delete
/*struct URL {
  std::string scheme;
  std::string userinfo;
  std::string host;
  std::string path;//!
  std::string raw_path;
  std::string raw_query;
};*/

typedef struct sServerInformation{
	//int port;
	//string ip;
	//string serverName;
	map<int, string> error_pages;//0 default
	size_t limit_size;//-1?64
	bool	autoindex;//0
	string file_request_if_dir;//default
	int redirection_status_code;//
	string redirection_url;//
	string name_file;//url_after_alias!
	map<string, int>	accepted_methods;//map->set
	string route_for_uploaded_files;//where 
	//cgi struct


}tServerInformation;

typedef struct sLocation{
	//string locationMask;
	//char	method;//mask
	//method 	//limit_except
	//string	root;///root
	//string	autoindex;//on or off
	//file if request is directory
	//map<string, string> cgiMap;
	map<int, string> error_pages;//0 default
	size_t limit_size;//-1?64
	bool	autoindex;//0
	string file_request_if_dir;//default
	int redirection_status_code;//
	string redirection_url;//
	string name_file;//url_after_alias!
	map<string, int>	accepted_methods;//map->set
	string route_for_uploaded_files;
}tLocation;

typedef struct	sServer{
	int					port;
	string				listen;
	string				ip;
	list<string>		server_name;
	map<int, string>	error_page;
	size_t				client_max_body_size;
	vector<Location>	locationMap;
}				tServer;

class WebserverConf{
	//
	//using std::string;
	private:
		WebserverConf(WebserverConf const &copy);
		WebserverConf &operator=(WebserverConf const &eq);
		void	readConfFile(const char *confFileName);
		void	fileToListLine(const char *confFileName);
		void	allLinesToToken();
		void	checkToken();
		void	tokenToServerMap();
		void	fillOneServer(std::list<std::string>::iterator	itList);
		void	fillMapServer(tServer &server);
		void	setListen(std::list<std::string>::iterator &itList, tServer	&server);
		void	setServer_name(std::list<std::string>::iterator &itList, tServer &server);
		void	setError_page(std::list<std::string>::iterator &itList, tServer	&server);
		void	setLocation(std::list<std::string>::iterator &itList, tServer	&server);
		void	setClient_max_body_size(std::list<std::string>::iterator &itList, tServer	&server);
		//std::vector<tServer> serverVector;//
		map<int, map<string, map<string, tServer *> > > serverMap;
		set<tServer *> pointerToServerSet;
		std::list<std::string> fileLineToList;
		std::list<std::string> tokenList;//separate ' ' or ';' or '{' or '}'

		void printServerMap();


	public:
		WebserverConf(char const *name = "webserver.conf");
		virtual ~WebserverConf();

		tServerInformation chooseServer(URL url);
};

#endif
}