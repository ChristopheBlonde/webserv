/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon           <skibidi@ohio.sus>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1833/02/30 06:67:85 by glaguyon          #+#    #+#             */
/*   Updated: 2025/03/20 16:21:53 by cblonde          ###   ########.fr       */
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
	std::map<int, std::vector<Server *> >::iterator socket;
	for (socket = serverFds.begin(); socket != serverFds.end(); socket++)
		std::cout << RED << "Server: fd: " << socket->first
			<< RESET << std::endl;
//not supposed to happen
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

bool	checkEndOfFile(std::string str)
{
	size_t		index;
	std::string	headers;
	std::string body;
	size_t		len;
	size_t		start;
	size_t		end;
	size_t		body_start;

	index = str.find("\r\n\r\n");
	if (index == std::string::npos)
		return (false);
	body_start = index + 4;
	headers = str.substr(0, index + 4);
	index = headers.find("Content-Length: ");
	if (index != std::string::npos)
	{
		start = index + 16;
		end	= headers.find("\r\n", start);
		if (end == std::string::npos)
			return (false);
		len = std::atoi(headers.substr(start, end - start).c_str());
		if (str.size() >= body_start + len)
			return (true);
		else
			return (false);
	}
	index = headers.find("Transfer-Encoding: chunked");
	if (index == std::string::npos)
		return (true);
	index = str.find("0\r\n\r\n");
	if (index == std::string::npos)
		return (false);
	else
		return (true);
}

void	Cluster::handleRequests(struct pollfd &fd)
{
	int		readByte;
	char	buffer[BUFFER_SIZE];

	if (!(fd.revents & POLLIN))
		return ;
	readByte = recv(fd.fd, buffer, BUFFER_SIZE - 1, 0);
	if (readByte < 0)
		return ;
	if (readByte > 0)
		buffer[readByte] = '\0';
	requests[fd.fd] += buffer;
	if (checkEndOfFile(requests[fd.fd]))
	{
		std::cout << CYAN << requests[fd.fd] << std::endl << RESET;
		Requests req(requests[fd.fd]);
		std::cout << GREEN << "Args pass to getServer: host: "<< req.getHost()
			<< " socket: " << fd.fd << RESET << std::endl;
		Server srv = getServer(3, req.getHost());
		Route  rt = getRoute(srv, req.getPath());
		req.setConf(rt);
		req.checkConf();
		Response *res = new Response(req);
		res->setSocket(fd.fd);
		std::cout << GREEN << "new Request create with Socket: "
			<< res->getSocket() << RESET << std::endl;
		ress[fd.fd] = res;
		requests.erase(fd.fd);	
	}
}

void	Cluster::handleFiles(void)
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
void	Cluster::run()
{
	if (poll(fds.data(), fds.size(), 1000) < 0)
		throw std::runtime_error("poll error");

	addClients();
	//manage requests, clients, cgi

	std::vector<std::map<int, Client>::iterator>	destroyList;
	//XXX poc
//	for (std::map<int, Client>::iterator it = clients.begin(); it != clients.end(); ++it)
	for(size_t i = 0; i < fds.size(); i++)
	{
		//std::cout << it->first << "\n";
		struct pollfd pfd = getPollFd(fds[i].fd);

		if (pfd.revents & (POLLERR | POLLHUP))
		{
			//? close the client ?
		}
		std::map<int, Response *>::iterator  res;
		std::map<int, Response *>::iterator  file;
		res = ress.find(pfd.fd);
		file = files.find(pfd.fd);
		if (res != ress.end())
		{
			if (!res->second->handleInOut(pfd))
			{
				std::cout << "dans delete" << std::endl;
				delete res->second;
				ress.erase(res);
				//_fds.erase(_fds.begin() + i--);
			}
		}
		else if (file != files.end())
		{
			if (!file->second->handleInOut(pfd))
			{
				close (file->first);
				files.erase(file);
			fds.erase(fds.begin() + i--);
			}
		}
		else
			handleRequests(pfd);
		handleFiles();
	}
	for (std::vector<std::map<int, Client>::iterator>::iterator it = destroyList.begin();
		it < destroyList.end(); ++it)
		clients.erase(*it);

}
