/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon           <skibidi@ohio.sus>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1833/02/30 06:67:85 by glaguyon          #+#    #+#             */
/*   Updated: 2025/03/16 18:03:00 by glaguyon         ###   ########.fr       */
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
}

Server	&Cluster::getServer(int fd, const std::string &host)
{
	std::map<int, std::vector<Server *> >::iterator it = serverFds.find(fd);
	
	if (it == serverFds.end())
		throw std::runtime_error("socket fd not in fd map");
	
	std::vector<Server *>	currServers = it->second;
	
	for (std::vector<Server *>::iterator it = currServers.begin(); it < currServers.end(); ++it)
	{
		if ((*it)->getNames().find(host) != (*it)->getNames().end())
			return **it;
	}
	return *(currServers[0]);
}

//match via longer prefix
Route	&Cluster::getRoute(Route &r, const std::string &path)
{
	std::vector<Route>	&currRoutes = r.getRoutes();
	Route			*longestRoute = &r;
	size_t			routeLen = 0;

	for (std::vector<Route>::iterator it = currRoutes.begin(); it < currRoutes.end(); ++it)
	{
		std::string	routeName = it->getName();
		
		if (routeName.size() > routeLen && routeName == path.substr(0, routeName.size()))
		{
			routeLen = routeName.size();
			longestRoute = &*it;
		}
	}
	if (longestRoute == &r)
		return r;
	return getRoute(*longestRoute, path);
}
