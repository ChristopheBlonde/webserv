/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Route.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon           <skibidi@ohio.sus>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1833/02/30 06:67:85 by glaguyon          #+#    #+#             */
/*   Updated: 2025/03/11 00:45:33 by glaguyon         ###   ########.fr       */
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

void	Route::setRoot(const std::string &s)
{
	root = s;
}

void	Route::addMethod(const std::string &s)
{
	if (acceptedMethods.find(s) != acceptedMethods.end())
		throw std::runtime_error("duplicate method: " + s);
	acceptedMethods.insert(s);
}

void	Route::addIndex(const std::string &s)
{
	indexFiles.push_back(s);
}

void	Route::setAutoindex(bool b)
{
	autoindex = b;
	autoindexAssigned = true;
}
