/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Route.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon           <skibidi@ohio.sus>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1833/02/30 06:67:85 by glaguyon          #+#    #+#             */
/*   Updated: 2025/02/22 20:54:49 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef ROUTE_HPP
# define ROUTE_HPP

class Route
{
	std::string				name;
	std::vector<std::string>		acceptedMethods;
	std::string				root;
	std::vector<std::string>		indexFiles;
	bool					autoindex;
	std::map<std::string, std::string>	cgi;
	std::string				uploads;
	std::string				redirection;
	
	//locations in locations
	Route					routes;

	public:
	Route();
	~Route();
};

#endif // ROUTE_HPP
