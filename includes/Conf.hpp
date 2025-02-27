/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Conf.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 18:42:46 by glaguyon          #+#    #+#             */
/*   Updated: 2025/02/27 18:21:24 by glaguyon         ###   ########.fr       */
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
#include <arpa/inet.h>
#include "Route.hpp"

#define DEF_ERROR_PAGE(page)	"<html>\r\n" \
				"<head><title>" page "</title></head>\r\n" \
				"<body>\r\n" \
				"<center><h1>" page "</h1></center>\r\n" \
				"<center>toile serviteur</center>\r\n" \
				"</body></html>\r\n"

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
	Route	*addRoute(const std::string &name);
};

#endif // CONF_HPP
