/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon           <skibidi@ohio.sus>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1833/02/30 06:67:85 by glaguyon          #+#    #+#             */
/*   Updated: 2025/03/14 18:46:29 by glaguyon         ###   ########.fr       */
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

	//TODO
	Server	&getServer(int fd, const std::string &host);//
	Route	&getRoute(Server &s, const std::string &path);//
	//une fonction run qui appelle poll
	//get requests (poll avec clients)
		//renvoie un fd mais aussi des infos sur le client
};

#endif // CLUSTER_HPP
