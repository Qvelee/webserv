/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   WebservConf.hpp                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelisabe <nelisabe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/18 23:24:54 by bbenny            #+#    #+#             */
/*   Updated: 2021/07/15 13:25:50 by nelisabe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERVCONF_HPP
#define WEBSERVCONF_HPP

#include <string>
#include <map>
#include <list>
#include <vector>
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

typedef struct sServerInformation{

	map<int, string>	error_pages;
	size_t				limit_size;
	bool				autoindex;//0
	string				file_request_if_dir;//default
	int					redirection_status_code;
	string				redirection_url;
	string				name_file;//url_after_alias!
	map<string, int>	accepted_methods;//map->set
	string				route_for_uploaded_files;//where
	bool				is_cgi;
	map<string, string> cgi;
}tServerInformation;

typedef struct sLocation{
	map<int, string>	error_pages;
	size_t				limit_size;//100000
	bool				autoindex;//0
	string				file_request_if_dir;//default
	int					redirection_status_code;
	string				redirection_url;//default
	string				name_file;//url_path_after_alias!
	map<string, int>	accepted_methods;//map->set?
	string				route_for_uploaded_files;
	string				filename_cgi;
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
	private:
	
		WebserverConf(WebserverConf const &copy);
		WebserverConf &operator=(WebserverConf const &eq);
		
		void	readConfFile(const char *confFileName);
		void	fileToListLine(const char *confFileName);
		void	allLinesToToken();
		void	checkToken();
		void	tokenToServerMap();
		void	fillOneServer(list<string>::iterator	itList);
		void	fillMapServer(tServer &server, map<int, string> &map_ip_port);
		void	setListen(list<string>::iterator &itList, tServer	&server, map<int, string> &map_ip_port);
		void	setServer_name(list<string>::iterator &itList, tServer &server, map<int, string> &map_ip_port);
		void	setError_page(list<string>::iterator &itList, tServer	&server, map<int, string>  &map_ip_port);
		void	setLocation(list<string>::iterator &itList, tServer	&server, map<int, string> &map_ip_port);
		void	setClient_max_body_size(list<string>::iterator &itList, tServer&server, map<int, string> &map_ip_port);

		map<int, map<string, tServer > > serverMap;//getter нужен
		list<string> fileLineToList;
		list<string> tokenList;//separate ' ' or ';' or '{' or '}'
		map<int, string> map_global_port_ip;

	public:
		
		map<int, map<string, tServer > >  const &getServerMap() const;//getter
		map<int, string >   const &getPorts() const ;//getter
		WebserverConf(char const *name = "webserver.conf");
		virtual ~WebserverConf();

};

tServerInformation chooseServer(http::url::URL url, map<string, tServer >  tmp);

}

#endif
