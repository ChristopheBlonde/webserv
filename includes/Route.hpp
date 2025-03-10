/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Route.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon           <skibidi@ohio.sus>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1833/02/30 06:67:85 by glaguyon          #+#    #+#             */
/*   Updated: 2025/03/10 23:11:06 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ROUTE_HPP
# define ROUTE_HPP

#include <map>
#include <vector>
#include <string>
#include <stdexcept>
#include "ExceptionTemplate.hpp"

class Route
{
	std::string				name;
	std::vector<std::string>		acceptedMethods;
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
};

#endif // ROUTE_HPP
