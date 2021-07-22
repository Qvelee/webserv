/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   IIOController.hpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelisabe <nelisabe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/21 19:24:12 by nelisabe          #+#    #+#             */
/*   Updated: 2021/07/21 20:40:57 by nelisabe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef IOCONTROLLER_HPP
# define IOCONTROLLER_HPP

class IIOController
{
	public:
		enum IOMode
		{
			READ = 0,
			WRITE
		};

		virtual bool	Wait() = 0;
		virtual void	AddFDToWatch(int fd, IOMode mode) = 0;
		virtual bool	CheckIfFDReady(int fd, IOMode mode) = 0;
};

#endif
