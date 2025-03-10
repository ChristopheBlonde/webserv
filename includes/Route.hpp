/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Route.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon           <skibidi@ohio.sus>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1833/02/30 06:67:85 by glaguyon          #+#    #+#             */
/*   Updated: 2025/03/11 00:45:05 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ROUTE_HPP
# define ROUTE_HPP

#include <map>
#include <set>
#include <vector>
#include <string>
#include <stdexcept>
#include <iostream>
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
	std::string				uploads;
	std::string				redirection;
	
	//locations in locations
	std::vector<Route>			routes;

	public:
	Route(const std::string &name);
	~Route();
	Route	*addRoute(const std::string &name);
	std::string	getName();
	void	setRoot(const std::string &s);
	void	addMethod(const std::string &s);
	void	addIndex(const std::string &s);
	void	setAutoindex(bool b);
};

#endif // ROUTE_HPP
