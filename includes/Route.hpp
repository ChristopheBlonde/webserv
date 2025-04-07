/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Route.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon           <skibidi@ohio.sus>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1833/02/30 06:67:85 by glaguyon          #+#    #+#             */
/*   Updated: 2025/04/07 20:47:22 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ROUTE_HPP
# define ROUTE_HPP

#include <map>
#include <set>
#include <vector>
#include <string>
#include <stdexcept>
#include "ExceptionTemplate.hpp"

class Route
{
	protected:
	std::string				name;
	std::set<std::string>			acceptedMethods;
	std::string				root;
	std::pair<std::string, std::string>	alias;
	std::vector<std::string>		indexFiles;
	bool					autoindex, autoindexAssigned;
	std::map<std::string, std::string>	cgi;
	std::string				redirection;
	std::string				uploadDir;
	size_t					maxBodySize;
	bool					maxBodySizeSet;
	
	//locations in locations
	std::vector<Route>			routes;

	public:
	Route(const std::string &name);
	~Route();
	Route					*addRoute(const std::string &name);
	std::string				getName();
	std::pair<std::string, std::string>	getAlias();
	void					setAlias(const std::string &n,
							const std::string &s);
	std::string				getRoot();
	void					setRoot(const std::string &s);
	std::string				getAliasedPart();
	std::string				getMount();
	std::set<std::string>			&getAcceptedMethods();
	void					addMethod(const std::string &s);
	std::vector<std::string>		&getIndex();
	void					addIndex(const std::string &s);
	bool					isAutoindexAssigned();
	bool					getAutoindex();
	void					setAutoindex(bool b);
	std::map<std::string, std::string>	&getCgi();
	void					addCgi(const std::string &ext,
							const std::string &exec);
	std::string				getRedirection();
	void					setRedirection(const std::string &s);
	std::string				getUploadDir();
	void					setUploadDir(const std::string &s);
	size_t					getMaxSize();
	void					setMaxSize(size_t size);
	bool					isMaxSizeSet();
	std::vector<Route>			&getRoutes();
};

#endif // ROUTE_HPP
