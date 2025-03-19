/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon           <skibidi@ohio.sus>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1833/02/30 06:67:85 by glaguyon          #+#    #+#             */
/*   Updated: 2025/03/16 20:23:44 by glaguyon         ###   ########.fr       */
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

//not supposed to happen
//	if (it == serverFds.end())
//		throw std::runtime_error("socket fd not in fd map");
	
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
		std::cout << pfd.fd << " client found\n";
		if (pfd.fd == -1)
			continue;
		pfd.events = POLLIN | POLLOUT;
		fds.push_back(pfd);
		clients.insert(std::make_pair(pfd.fd, Client(pfd.fd)));
	}
}

void	Cluster::run()
{
	if (poll(fds.data(), fds.size(), 1000) < 0)
		throw std::runtime_error("poll error");

	addClients();
	//manage requests, clients, cgi

	std::vector<std::map<int, Client>::iterator>	destroyList;
	//XXX poc
	for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it)
	{
		//std::cout << it->first << "\n";
		if (getPollFd(it->first).revents & (POLLERR | POLLHUP))
		{
			//? close the client ?
		}
		if (getPollFd(it->first).revents & POLLIN)
		{
			char		request[1024];
			int			read;

			read = recv(it->first, request, 1023, 0);
			if (read == 0)
			{
				std::cout << "client " << it->first << " disconnected\n";
				close(it->first);//use client close instead
				destroyList.push_back(it);
				//clients.erase(it);
				fds.erase(std::find(fds.begin(), fds.end(), PollFd(it->first)));
			}
			request[read] = '\0';
			std::cout << "client " << it->first << " " << request << "\n";
		}
	}
	for (std::vector<std::map<int, Client>::iterator>::iterator it = destroyList.begin();
		it < destroyList.end(); ++it)
		clients.erase(*it);

}

//void	Server::run(void)
//{
//	int			check = -1;
//	char		request[1024];
//	int			read;
//	
//	get_client_maybe();//FIXME this needs to be polled
//	if (_fds.size())
//		check = poll(_fds.data(), _fds.size(), 5000);
//	if (check < 0)
//		return ;
//	for (size_t i = 0; i < _fds.size(); i++)
//	{
//		if (_fds[i].fd == -1)
//			continue ;
//		if (_fds[i].revents & (POLL_ERR | POLL_HUP))
//			throw Server::ServerException(
//					std::string("ERROR: poll: ") + strerror(errno));
//		if (_fds[i].revents & (POLL_IN))
//		{
//			read = recv(_fds[i].fd, request, 1023, 0);
//			if (read == 0)
//			{
//				std::cout << "client " << _fds[i].fd << " disconnected\n";
//				_fds.erase(_fds.begin() + i--);
//				break;
//			}
//			request[read] = '\0';
//			std::cout << "client " << _fds[i].fd << " " << CYAN << request << RESET << std::endl;
//		}
//	}
//	return ;
//}
