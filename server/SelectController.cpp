/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   SelectController.cpp                               :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: nelisabe <nelisabe@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/21 19:37:40 by nelisabe          #+#    #+#             */
/*   Updated: 2021/07/22 21:28:59 by nelisabe         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

# include "SelectController.hpp"

SelectController::SelectController() : _max_fd(0)
{
	FD_ZERO(&_read_set);
	FD_ZERO(&_write_set);
}

SelectController::SelectController(const SelectController &) { }

SelectController::~SelectController() { };

SelectController	&SelectController::operator=(const SelectController &)
{ return *this; }

bool	SelectController::Wait(void)
{
	if (select(_max_fd + 1, &_read_set, &_write_set, NULL, NULL) == -1)
		return FAILURE;
	return SUCCESS;
}

void	SelectController::AddFDToWatch(int fd, IOMode mode)
{
	if (mode == READ)
		FD_SET(fd, &_read_set);
	if (mode == WRITE)
		FD_SET(fd, &_write_set);
	_max_fd = fd > _max_fd ? fd : _max_fd;
}

bool	SelectController::CheckIfFDReady(int fd, IOMode mode)
{
	if (mode == READ)
		if (FD_ISSET(fd, &_read_set))
			return true;
	if (mode == WRITE)
		if (FD_ISSET(fd, &_write_set))
			return true;
	return false;
}

void	SelectController::Clear(void)
{
	FD_ZERO(&_read_set);
	FD_ZERO(&_write_set);
	_max_fd = 0;
}
