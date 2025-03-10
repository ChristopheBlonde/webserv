/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Conf.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon           <skibidi@ohio.sus>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1833/02/30 06:67:85 by glaguyon          #+#    #+#             */
/*   Updated: 2025/03/11 00:26:13 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Conf.hpp"

Conf::Conf() :
	Route("you shouldn't be seeing this :("),
	ip(0),
	port(htons(8080)),
	names(),
	maxBodySize(1e6),
	errorPages()
{
}

Conf::~Conf()
{
}

void	Conf::setIp(uint32_t n)
{
	ip = n;
}

void	Conf::setPort(uint16_t n)
{
	port = n;
}

void	Conf::addName(const std::string &s)
{
	if (names.find(s) != names.end())
		throw std::runtime_error(s);
	names.insert(s);
}

void	Conf::setErrorPage(unsigned code, const std::string &page)
{
	errorPages[code] = page;
}

void	Conf::setMaxSize(size_t size)
{
	maxBodySize = size;
}
