/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Errors.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelisabe <nelisabe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/22 19:30:31 by nelisabe          #+#    #+#             */
/*   Updated: 2021/07/22 21:43:48 by nelisabe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "Errors.hpp"

bool	Error(const std::string &error, bool print_errno)
{
	std::cerr << "Error: " << error;
	if (print_errno)
		std::cerr << ": " << strerror(errno);
	std::cerr << std::endl;
	return FAILURE;
}
