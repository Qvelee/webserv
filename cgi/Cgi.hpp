/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: bbenny <bbenny@student.21-school.ru>       +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/04/24 21:50:29 by bbenny            #+#    #+#             */
/*   Updated: 2021/04/24 21:50:30 by bbenny           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
# define CGI_HPP

#include <map>
#include <string>

namespace config{

using std::map;
using std::string;

class Cgi{
	private:
	public:
		Cgi();
		virtual ~Cgi();
		Cgi(Cgi const &copy);
		Cgi &operator=(Cgi const &eq);
		map<string, string> param;
};

}

#endif

