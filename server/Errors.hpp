/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Errors.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelisabe <nelisabe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/22 19:30:32 by nelisabe          #+#    #+#             */
/*   Updated: 2021/07/22 21:59:59 by nelisabe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ERRORS_HPP
# define ERRORS_HPP

# include <string>
# include <string.h>
# include <iostream>

# define SUCCESS false
# define FAILURE true

bool	Error(const std::string &error, bool print_errno = true);

template<typename T>
bool	Error(const std::string &error, T parameter,\
			bool print_errno)
{
	size_t	replacement_pos;

	std::cerr << "Error: ";
	if ((replacement_pos = error.find("{-}", 0)) != std::string::npos)
	{
		std::cerr << error.substr(0, replacement_pos);
		std::cerr << parameter;
		std::cerr << error.substr(replacement_pos + 3, error.size());
	}
	else
		std::cerr << error;
	if (print_errno)
		std::cerr << ": " << strerror(errno);
	std::cerr << std::endl;
	return FAILURE;
}

#endif
