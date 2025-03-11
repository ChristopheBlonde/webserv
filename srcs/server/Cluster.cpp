/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon           <skibidi@ohio.sus>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1833/02/30 06:67:85 by glaguyon          #+#    #+#             */
/*   Updated: 2025/03/11 23:00:26 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Cluster.hpp"

const std::string		Cluster::defaultRoot = "/html/";
const std::string 		Cluster::allowedMethods[3] = {"GET", "POST", "DELETE"};
const size_t			Cluster::allowedMethodsSize =
		sizeof(Cluster::allowedMethods) / sizeof(Cluster::allowedMethods[0]);

Cluster::Cluster(const std::string &filename)
{
	ConfParser	parser(*this, filename);

	//check si un server a ete ajoute, sinon erreur ? ou en ajouter 1 par defaut
	try
	{
		parser.parseConf();
	}
	catch (std::exception &e)
	{
		std::cerr << filename << ":"
			<< parser.getLine() + 1 << ":" << parser.getI() + 1 << ": ";
		throw;
	}
	parser.fillBlanks();
	//init poll, fds etc
}

Cluster::~Cluster()
{
}

Server			*Cluster::addServer()
{
	servers.push_back(Server(servers.size()));
	return &servers.back();
}

std::vector<Server>	&Cluster::getServers()
{
	return servers;
}
