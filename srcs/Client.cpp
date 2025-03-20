/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon           <skibidi@ohio.sus>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1833/02/30 06:67:85 by glaguyon          #+#    #+#             */
/*   Updated: 2025/03/20 17:42:09 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

Client::Client() :
	on(false),
	fd(-1)
{
}

Client::Client(int fd) :
	on(false),
	fd(fd)
{
}

Client::~Client()
{
	if (!on)
		return;
	close(fd);
}

void	Client::init()
{
	on = true;
}

void	Client::requestAppend(const std::string &s)
{
	currRequest += s;
}

Requests	Client::getRequest()
{
	Requests	req(currRequest);

	currRequest = "";
	return req;
}
