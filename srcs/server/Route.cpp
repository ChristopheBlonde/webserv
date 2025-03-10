/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Route.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon           <skibidi@ohio.sus>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1833/02/30 06:67:85 by glaguyon          #+#    #+#             */
/*   Updated: 2025/03/10 23:25:01 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Route.hpp"

Route::Route(const std::string &name) :
	name(name),
	autoindex(true),
	autoindexAssigned(false)
{
}

Route::~Route()
{
}

Route	*Route::addRoute(const std::string &name)
{
	for (std::vector<Route>::iterator it = routes.begin(); it != routes.end(); ++it)
	{
		if (it->name == name)
			throw std::runtime_error("duplicate route " + name);
	}
	routes.push_back(Route(name));
	return &routes.back();
}

std::string	Route::getName()
{
	return name;
}
