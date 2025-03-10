/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Conf.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon           <skibidi@ohio.sus>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1833/02/30 06:67:85 by glaguyon          #+#    #+#             */
/*   Updated: 2025/03/10 19:12:23 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Conf.hpp"

Conf::Conf() :
	ip(0),
	port(htons(8080)),
	names(1, ""),
	maxBodySize(1e6),
	errorPages(),
	defRoute("you shouldn't be seeing this :("),
	routes()
{
}

Conf::~Conf()
{
}

Route	*Conf::addRoute(const std::string &name)
{
	routes.push_back(Route(name));
	return &routes.back();
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
	if (std::find(names.begin(), names.end(), s) != names.end())
		throw std::runtime_error(s);
	names.push_back(s);
}
