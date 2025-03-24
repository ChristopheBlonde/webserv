/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Route.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon           <skibidi@ohio.sus>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1833/02/30 06:67:85 by glaguyon          #+#    #+#             */
/*   Updated: 2025/03/24 04:39:09 by glaguyon         ###   ########.fr       */
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

std::string	Route::getRoot()
{
	return root;
}

std::set<std::string>	&Route::getAcceptedMethods()
{
	return acceptedMethods;
}

void	Route::addMethod(const std::string &s)
{
	acceptedMethods.insert(s);
}

std::vector<std::string>	&Route::getIndex()
{
	return indexFiles;
}

void	Route::addIndex(const std::string &s)
{
	indexFiles.push_back(s);
}

bool	Route::getAutoindex()
{
	return autoindex;
}

bool	Route::isAutoindexAssigned()
{
	return autoindexAssigned;
}

void	Route::setAutoindex(bool b)
{
	autoindex = b;
	autoindexAssigned = true;
}

std::map<std::string, std::string>	&Route::getCgi()
{
	return cgi;
}

void	Route::addCgi(const std::string &ext, const std::string &exec)
{
	cgi[ext] = exec;
}

std::string	Route::getRedirection()
{
	return redirection;
}

void	Route::setRedirection(const std::string &s)
{
	redirection = s;
}
std::string	Route::getUploadDir()
{
	return uploadDir;
}

void	Route::setUploadDir(const std::string &s)
{
	uploadDir = s;
}

std::vector<Route>	&Route::getRoutes()
{
	return routes;
}
