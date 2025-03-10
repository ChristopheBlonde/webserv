/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Conf.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 18:42:46 by glaguyon          #+#    #+#             */
/*   Updated: 2025/03/10 19:07:37 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONF_HPP
# define CONF_HPP

#include <map>
#include <vector>
#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <limits>
#include <algorithm>
#include <stdint.h>
#include <arpa/inet.h>
#include "Route.hpp"

class Conf
{
	protected:
	//for the server
	uint32_t				ip;
	uint16_t				port;
	std::vector<std::string>		names;
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
	void	setIp(uint32_t ip);
	void	setPort(uint16_t port);
	void	addName(const std::string &s);
};

#endif // CONF_HPP
