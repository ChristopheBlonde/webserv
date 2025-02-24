/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Conf.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 18:42:46 by glaguyon          #+#    #+#             */
/*   Updated: 2025/02/24 22:51:26 by glaguyon         ###   ########.fr       */
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

# define DEFAULT_CONF "./conf/def.conf"

class Conf
{
	//exceptions
	EXC_CLASS(ConfNoFileException);
	EXC_CLASS(ConfUnexpectedCloseException);
	EXC_CLASS(ConfUnexpectedOpenException);
	EXC_CLASS(ConfMissingCloseException);

	//static variables
	static const std::string		spaces;
	static const char			openBlock;
	static const char			closeBlock;

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

	
	void	parseConfigFile(std::ifstream &file);

	public:
	Conf(const std::string &filename);
	~Conf();
	//getroute ?
	//runcgi, dossier correct
};

#endif // CONF_HPP
