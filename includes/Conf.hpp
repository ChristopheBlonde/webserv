/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Conf.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 18:42:46 by glaguyon          #+#    #+#             */
/*   Updated: 2025/03/11 23:25:57 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONF_HPP
# define CONF_HPP

#include <map>
#include <set>
#include <string>
#include <iostream>
#include <fstream>
#include <cstdlib>
#include <limits>
#include <algorithm>
#include <stdint.h>
#include <arpa/inet.h>
#include "Route.hpp"

class Conf : public Route
{
	protected:
	uint32_t				ip;
	uint16_t				port;
	std::set<std::string>			names;
	size_t					maxBodySize;
	std::map<unsigned, std::string>		errorPages;

	public:
	Conf();
	~Conf();
	void			setIp(uint32_t ip);
	void			setPort(uint16_t port);
	std::set<std::string>	&getNames();
	void			addName(const std::string &s);
	void			setErrorPage(unsigned code, const std::string &page);
	void			setMaxSize(size_t size);
};

#endif // CONF_HPP
