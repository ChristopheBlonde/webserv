/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Conf.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon           <skibidi@ohio.sus>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1833/02/30 06:67:85 by glaguyon          #+#    #+#             */
/*   Updated: 2025/03/25 19:15:46 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Conf.hpp"

Conf::Conf() :
	Route("default server route"),
	ip(0),
	port(htons(8080)),
	ipStr("0.0.0.0"),
	portStr("8080"),
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
	struct in_addr addr = {.s_addr = n};
	char                    ipStrChar[INET_ADDRSTRLEN];

	ip = n;
        inet_ntop(AF_INET, &addr, ipStrChar, sizeof(ipStrChar));
        ipStr = ipStrChar;
}

void	Conf::setPort(uint16_t n)
{
        std::stringstream       ss;
	
	port = n;
        ss << ntohs(port);
        portStr = ss.str();
}

std::string	Conf::getIpStr()
{
	return ipStr;
}

std::string	Conf::getPortStr()
{
	return portStr;
}

std::set<std::string>	&Conf::getNames()
{
	return names;
}

void	Conf::addName(const std::string &s)
{
	names.insert(s);
}

std::string	Conf::getErrorPage(int code)
{
	return errorPages[code];
}

void	Conf::setErrorPage(unsigned code, const std::string &page)
{
	errorPages[code] = page;
}

size_t	Conf::getMaxSize()
{
	return maxBodySize;
}

void	Conf::setMaxSize(size_t size)
{
	maxBodySize = size;
}
