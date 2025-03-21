/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Route.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon           <skibidi@ohio.sus>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1833/02/30 06:67:85 by glaguyon          #+#    #+#             */
/*   Updated: 2025/03/21 09:57:30 by cblonde          ###   ########.fr       */
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
	std::vector<std::string>		indexFiles;
	bool					autoindex, autoindexAssigned;
	std::map<std::string, std::string>	cgi;
	std::string				redirection;
	std::string				uploadDir;
	
	//locations in locations
	std::vector<Route>			routes;

	public:
	Route(void);
	Route(const std::string &name);
	~Route();
	Route					*addRoute(const std::string &name);
	std::string				getName();
	std::string				getRoot();
	void					setRoot(const std::string &s);
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
	std::vector<Route>			&getRoutes();
};

#endif // ROUTE_HPP
