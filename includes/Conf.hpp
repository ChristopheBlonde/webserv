/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Conf.hpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/22 18:42:46 by glaguyon          #+#    #+#             */
/*   Updated: 2025/02/22 20:45:55 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONF_HPP
# define CONF_HPP

enum	methods
{
	GET = 1U << 0U;
	POST = 1U << 1U;
	DELETE = 1U << 2U;
};

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
	Route					routes;
	
	public:
	Conf(const std::string &filename);
	~Conf();
	//getroute ?
	//runcgi, dossier correct
};

#endif // CONF_HPP
