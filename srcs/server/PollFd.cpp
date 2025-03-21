/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   PollFd.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon           <skibidi@ohio.sus>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1833/02/30 06:67:85 by glaguyon          #+#    #+#             */
/*   Updated: 2025/03/20 17:47:45 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "PollFd.hpp"

PollFd::PollFd()
{
	fd = -1;
	events = 0;
	revents = 0;
}

PollFd::PollFd(int fd2)
{
	fd = fd2;
	events = 0;
	revents = 0;
}

bool	PollFd::operator==(const PollFd &p)
{
	return fd == p.fd;
}

PollFd::~PollFd()
{
}
