/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon           <skibidi@ohio.sus>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1833/02/30 06:67:85 by glaguyon          #+#    #+#             */
/*   Updated: 2025/03/20 22:06:12 by glaguyon         ###   ########.fr       */
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
	servers.push_back(Server());
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
		it = serverFds.find(clientFdToServFd[fd]);
	
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

PollFd	&Cluster::getPollFd(int fd)
{
	//fd should always be in poll
	return *std::find(fds.begin(), fds.end(), PollFd(fd));
}

void	Cluster::addClients()
{
	for (std::map<int, std::vector<Server *> >::iterator it = serverFds.begin();
		it != serverFds.end(); ++it)
	{
		if (!(getPollFd(it->first).revents & POLLIN))
			continue;

		PollFd	pfd;

		pfd.fd = accept(it->first, NULL, NULL);
		if (pfd.fd == -1)
			continue;
		pfd.events = POLLIN | POLLOUT;
		fds.push_back(pfd);
		clients.insert(std::make_pair(pfd.fd, Client(pfd.fd)));
		clients[pfd.fd].init();
		clientFdToServFd[pfd.fd] = it->first;
	}
}

void	Cluster::closeClient(int fd)
{
	clientCloseList.push_back(fd);
}

void	Cluster::destroyClients()
{
	for (std::vector<int>::iterator it = clientCloseList.begin();
		it < clientCloseList.end(); ++it)
	{
		clients.erase(clients.find(*it));
		clientFdToServFd.erase(clientFdToServFd.find(*it));
		fds.erase(std::find(fds.begin(), fds.end(), PollFd(*it)));
	}
}

void	Cluster::run()
{

	if (poll(fds.data(), fds.size(), POLL_TIMEOUT) < 0 && errno != EINTR)
		throw std::runtime_error("poll error");
	addClients();
	for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if (getPollFd(it->first).revents & POLLERR)
			clientCloseList.push_back(it->first);
		else if (getPollFd(it->first).revents & POLLIN)
			it->second.handleRequest();
		else if (getPollFd(it->first).revents & POLLIN)
			it->second.handleResponse(0);//TODO
	}
//	for(size_t i = 0; i < fds.size(); i++)
//	{
//		//std::cout << it->first << "\n";
//		PollFd pfd = getPollFd(fds[i].fd);
//
//		if (pfd.revents & POLLERR)
//		{//?}
//		//XXX il faut check poll in ici de preference
//		std::map<int, Response *>::iterator  res;
//		std::map<int, Response *>::iterator  file;
//		res = ress.find(pfd.fd);
//		file = files.find(pfd.fd);
//		if (res != ress.end())
//		{
//			if (!res->second->handleInOut(pfd))
//			{
//				std::cout << "dans delete" << std::endl;
//				delete res->second;
//				ress.erase(res);
//				//_fds.erase(_fds.begin() + i--);
//			}
//		}
//		else if (file != files.end())
//		{
//			if (!file->second->handleInOut(pfd))
//			{
//				close (file->first);
//				files.erase(file);
//			fds.erase(fds.begin() + i--);
//			}
//		}
//		else
//			handleRequests(pfd);
//		handleFiles();
//	}
	destroyClients();
}

void	Cluster::handleFiles(void)//
{
	PollFd	res;

	for (std::map<int, Response *>::iterator it = ress.begin();
			it != ress.end(); it++)
	{
		res.fd = it->second->getFileFd();
		if (res.fd < 0 || (files.find(res.fd) != files.end()))
			continue ;
		res.events = POLLIN;
		res.revents = 0;
		fds.push_back(res);
		files.insert(std::make_pair(res.fd, it->second));
	}
}
