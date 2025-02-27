/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Conf.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon           <skibidi@ohio.sus>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1833/02/30 06:67:85 by glaguyon          #+#    #+#             */
/*   Updated: 2025/02/27 18:22:50 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Conf.hpp"

Conf::Conf() :
	host(0),
	port(htons(8080)),
	name(""),
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
