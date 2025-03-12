/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon           <skibidi@ohio.sus>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1833/02/30 06:67:85 by glaguyon          #+#    #+#             */
/*   Updated: 2025/03/12 17:33:32 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLUSTER_HPP
# define CLUSTER_HPP

#include <vector>
#include <string>
#include <iostream>
#include <poll.h>
#include "Server.hpp"
#include "ConfParser.hpp"

class Cluster
{
	std::vector<Server>	servers;
	std::vector<pollfd>	fds;
	//std::map<fd, ??>	fdMap;
	//serveurs, clients, fichiers, cgi, envoi reponses, envoi cgi
	//liste de clients

	public:
	static const std::string	defaultRoot;
	static const std::string	allowedMethods[];
	static const size_t		allowedMethodsSize;
	
	Cluster(const std::string &filename);
	~Cluster();
	Server			*addServer();
	std::vector<Server>	&getServers();
	void			startServers();

	//fill blanks
	//init servers
	//try add clients
	//get requests (poll avec clients)
		//renvoie un fd mais aussi des infos sur le client
};

#endif // CLUSTER_HPP
