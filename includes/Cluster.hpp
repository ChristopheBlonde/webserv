/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon           <skibidi@ohio.sus>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1833/02/30 06:67:85 by glaguyon          #+#    #+#             */
/*   Updated: 2025/03/10 22:58:37 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLUSTER_HPP
# define CLUSTER_HPP

#include <vector>
#include <string>
#include <iostream>
#include "Server.hpp"
#include "ConfParser.hpp"

class Cluster
{
	std::vector<Server>	servers;
	
	//liste des fds de merde, server listen, clients, fichiers gros, cgi

	public:
	Cluster(const std::string &filename);
	~Cluster();
	Server			*addServer();

	//check location en double XXX
	//fill blanks
	//init servers
	//try add clients
	//get requests (poll avec clients)
		//renvoie un fd mais aussi des infos sur le client
};

#endif // CLUSTER_HPP
