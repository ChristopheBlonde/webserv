/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Conf.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 18:42:46 by glaguyon          #+#    #+#             */
/*   Updated: 2025/02/25 17:33:04 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONF_HPP
# define CONF_HPP

#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <stdint.h>
#include "Route.hpp"

class Conf
{
	//for the server
	uint32_t				host;
	uint16_t				port;
	std::string				name;
	size_t					maxBodySize;
	std::map<unsigned, std::string>		errorPages;

	//default in server block
	Route					defRoute;

	//routes ie locations
	std::vector<Route>			routes;

	public:
	Conf();
	~Conf();
	//getroute ?
	//runcgi, dossier correct
};

#endif // CONF_HPP
