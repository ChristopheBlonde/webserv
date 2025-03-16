/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon           <skibidi@ohio.sus>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1833/02/30 06:67:85 by glaguyon          #+#    #+#             */
/*   Updated: 2025/03/16 17:07:01 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLUSTER_HPP
# define CLUSTER_HPP

#include <vector>
#include <string>
#include <iostream>
#include "PollFd.hpp"
#include "Server.hpp"
#include "ConfParser.hpp"

class Cluster
{
	std::vector<Server>			servers;
	std::vector<PollFd>			fds;
	std::map<int, std::vector<Server *> >	serverFds;
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

	Server	&getServer(int fd, const std::string &host);
	Route	&getRoute(Route &r, const std::string &path);
	//get clients
	//une fonction run qui appelle polli


	//get requests (poll avec clients)
		//renvoie un fd mais aussi des infos sur le client
};

#endif // CLUSTER_HPP
