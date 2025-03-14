/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon           <skibidi@ohio.sus>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1833/02/30 06:67:85 by glaguyon          #+#    #+#             */
/*   Updated: 2025/03/14 19:00:30 by glaguyon         ###   ########.fr       */
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
	if (servers.size() == 0)
		addServer();
	parser.fillBlanks();
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

void	Cluster::startServers()
{
	PollFd	fd;

	for (std::vector<Server>::iterator it = servers.begin(); it < servers.end(); ++it)
	{
		fd.fd = it->start();
		fd.events = POLLIN;
		if (std::find(fds.begin(), fds.end(), fd) == fds.end())
			fds.push_back(fd);
		serverFds[fd.fd].push_back(&*it);
	}
	for (std::map<int, std::vector<Server *> >::iterator it = serverFds.begin(); it != serverFds.end(); ++it)
	{
		std::cout << it->first << "\n";
		for (std::vector<Server *>::iterator it2 = it->second.begin(); it2 < it->second.end(); ++it2)
		{
			std::cout << *(*it2)->getNames().begin() << "\n";
		}

	}
}
