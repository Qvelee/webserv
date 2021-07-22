/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SelectController.hpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelisabe <nelisabe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/21 19:37:23 by nelisabe          #+#    #+#             */
/*   Updated: 2021/07/21 20:26:02 by nelisabe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SELECTCTRL_HPP
# define SELECTCTRL_HPP

# include "IIOController.hpp"
# include <sys/select.h>
# include <unistd.h>

# define SUCCESS false
# define FAILURE true	

class SelectController : public IIOController
{
	public:
		SelectController();
		virtual ~SelectController();

		bool	Wait(void);
		void	AddFDToWatch(int fd, IOMode mode);
		bool	CheckIfFDReady(int fd, IOMode mode);
		void	Clear(void);
	private:
		SelectController(const SelectController &);
		
		SelectController	&operator=(const SelectController &);

		fd_set	_read_set;
		fd_set	_write_set;
		int		_max_fd;
};

#endif
