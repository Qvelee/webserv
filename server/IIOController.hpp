/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IIOController.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelisabe <nelisabe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/21 19:24:12 by nelisabe          #+#    #+#             */
/*   Updated: 2021/07/26 11:45:49 by nelisabe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IOCONTROLLER_HPP
# define IOCONTROLLER_HPP

# include <cstdlib>

class IIOController
{
	public:
		enum IOMode
		{
			READ = 0,
			WRITE
		};

		virtual bool	Wait(size_t timeout_seconds) = 0;
		virtual void	AddFDToWatch(int fd, IOMode mode) = 0;
		virtual bool	CheckIfFDReady(int fd, IOMode mode) = 0;
};

#endif
