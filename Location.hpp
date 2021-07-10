/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Location.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbenny <bbenny@student.21-school.ru>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/23 12:40:12 by bbenny            #+#    #+#             */
/*   Updated: 2021/04/23 12:40:14 by bbenny           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef LOCATION_HPP
# define LOCATION_HPP

#include <string>
#include <map>
#include <list>
#include <vector>
#include <array>
#include <iostream>

namespace config{

using std::map;
using std::string;
using std::vector;
using std::list;

class Location{
	private:
		
	public:
		Location();
		virtual ~Location();
		Location(Location const &copy);
		Location &operator=(Location const &eq);
		string				locationMask;
		string				root;///root
		string				alias;
		//map<string, string> cgiMap;
		bool	autoindex;//0
		map<string, int>	accepted_methods;//map->set
		string route_for_uploaded_files;
		int redirection_status_code;//
		string redirection_url;

		string file_request_if_dir;//default

		
		

		void	fillAll(list<string>::iterator &itList, std::list<std::string> tokenList);
		void	setAutoindex(list<string>::iterator &itList, std::list<std::string> tokenList);
		void	setRoot(list<string>::iterator &itList, std::list<std::string> tokenList);
		void	setAlias(list<string>::iterator &itList, std::list<std::string> tokenList);
		void	setMethod(list<string>::iterator &itList, std::list<std::string> tokenList);
		void	setRootUploadedFiles(list<string>::iterator &itList, std::list<std::string> tokenList);
		void	setRedirection(list<string>::iterator &itList, std::list<std::string> tokenList);
};
}
#endif
