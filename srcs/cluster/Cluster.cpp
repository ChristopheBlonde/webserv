/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon           <skibidi@ohio.sus>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1833/02/30 06:67:85 by glaguyon          #+#    #+#             */
/*   Updated: 2025/04/08 17:20:06 by glaguyon         ###   ########.fr       */
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
	destroyClients();
	deleteFds();
	for (std::map<int, Client>::iterator it = clients.begin();
		it != clients.end(); ++it)
		clientCloseList.push_back(it->first);
	destroyClients();
	deleteFds();
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
		
		if (routeName.size() > routeLen
			&& routeName == path.substr(0, routeName.size())
			&& (path[routeName.size()] == '/'
				|| path[routeName.size()] == '\0'
				|| routeName[routeName.size() - 1] == '/'))
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

		PollFd			pfd;
		struct sockaddr_in	addr;
		socklen_t		addrlen = sizeof(addr);

		pfd.fd = accept(it->first, (struct sockaddr *)&addr, &addrlen);
		if (pfd.fd == -1)
			continue;
		pfd.events = POLLIN | POLLOUT;
		fds.push_back(pfd);
		clients.insert(std::make_pair(pfd.fd,
					Client(pfd.fd, addr, this)));
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
	clientCloseList.clear();
}

void	Cluster::addFd(PollFd pfd)
{
	fds.push_back(pfd);
}

void	Cluster::removeFd(int fd)
{
	fdRemoveList.push_back(fd);
}

void	Cluster::deleteFds()
{
	for (std::vector<int>::iterator it = fdRemoveList.begin();
		it < fdRemoveList.end(); ++it)
		fds.erase(std::find(fds.begin(), fds.end(), PollFd(*it)));
	fdRemoveList.clear();
}

short	Cluster::getRevents(int fd)
{
	return getPollFd(fd).revents;
}

void	Cluster::removeSession(std::string id)
{
	std::map<std::string, std::string>::iterator it = sessionMap.find(id);

	if (it != sessionMap.end())
		sessionMap.erase(it);
}

std::string	Cluster::getSession(std::string id)
{
	return sessionMap[id];
}

void	Cluster::editSession(std::string key, std::string value)
{
	sessionMap[key] = value;
}

void	Cluster::editSession(std::pair<std::string, std::string> pair)
{
	sessionMap[pair.first] = pair.second;
}

void	Cluster::run()
{
	if (poll(fds.data(), fds.size(), POLL_TIMEOUT) < 0 && errno != EINTR)
		throw std::runtime_error("poll error");
	addClients();
	for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		if (getPollFd(it->first).revents & POLLERR)
			closeClient(it->first);
		else if (getPollFd(it->first).revents & POLLIN)
			it->second.handleRequest();
		else if (getPollFd(it->first).revents & POLLOUT)
			it->second.handleResponse();
	}
	destroyClients();
	deleteFds();
}
