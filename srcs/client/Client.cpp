/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon           <skibidi@ohio.sus>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1833/02/30 06:67:85 by glaguyon          #+#    #+#             */
/*   Updated: 2025/04/08 18:07:22 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Cluster.hpp"

Client::Client() :
	on(false),
	c(NULL),
	fd(-1),
	ip(-1),
	port(-1),
	hostName("")
{
	resetRequest();
}

Client::Client(int fd, struct sockaddr_in addr, Cluster *c) :
	on(false),
	c(c),
	fd(fd),
	ip(addr.sin_addr.s_addr),
	port(addr.sin_port),
	hostName("")
{
	char                    ipStrChar[INET_ADDRSTRLEN];
        std::stringstream       ss;
	struct addrinfo		hints;
	struct addrinfo		*res = 0;

        inet_ntop(AF_INET, &addr.sin_addr, ipStrChar, sizeof(ipStrChar));
        ipStr = ipStrChar;
        ss << ntohs(port);
        portStr = ss.str();
	memset(&hints, 0, sizeof(hints));
	hints.ai_flags = AI_CANONNAME;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	if (getaddrinfo(ipStrChar, portStr.c_str(), &hints, &res) == 0)
	{
		if (res->ai_canonname)
			hostName = res->ai_canonname;
		else
			hostName = "";
		freeaddrinfo(res);
	}
	else
		hostName = "";
}

uint64_t	Client::getIp()
{
	return ip;
}

std::string	Client::getIpStr()
{
	return ipStr;
}

uint16_t	Client::getPort()
{
	return port;
}

std::string	Client::getPortStr()
{
	return portStr;
}

std::string	Client::getHostName()
{
	return hostName;
}

void	Client::init()
{
	on = true;
}

void	Client::handleRequest()
{
	try
	{
		if (!receiveRequest(fd))
			return;
		Requests req(currRequestRaw, *this, c, fd);
		Response *res = new Response(req, *this,
				c->getServer(fd, req.getHost()));
		responses.push(res);
		resetRequest();
	}
	catch (std::exception &e)
	{
		c->closeClient(fd);
	}
}

void	Client::handleResponse()
{
	if (responses.empty())
		return;
	
	PollFd	pfd(fd);

	pfd.revents = c->getRevents(fd);
	//POLLOUT is on
	if (!responses.front()->handleInOut(pfd))
	{
		delete responses.front();
		responses.pop();
		removeFds();
		for (std::vector<int>::iterator it = responseFds.begin();
			it < responseFds.end(); ++it)
			responseFdRemoveList.push_back(*it);
		removeFds();
		return ;
	}
	removeFds();
	addFds();
	for (std::vector<int>::iterator it = responseFds.begin(); it < responseFds.end(); ++it)
	{
		PollFd		pfd(*it);

		pfd.revents = c->getRevents(*it);
		if (!responses.front()->handleInOut(pfd))
			responseFdRemoveList.push_back(*it);

	}
	removeFds();
	addFds();
}

void	Client::removeFds()
{
	for (std::vector<int>::iterator it = responseFdRemoveList.begin();
		it < responseFdRemoveList.end(); ++it)
	{
		close(*it);
		responseFds.erase(std::find(responseFds.begin(), responseFds.end(), *it));
		c->removeFd(*it);
	}
	responseFdRemoveList.clear();
}

Client::~Client()
{
	if (!on)
		return;
	close(fd);
	while (!responses.empty())
	{
		delete responses.front();
		responses.pop();
	}
	removeFds();
	for (std::vector<int>::iterator it = responseFds.begin(); it < responseFds.end(); ++it)
		responseFdRemoveList.push_back(*it);
	removeFds();
}

void	Client::addResponseFd(PollFd pfd)
{
	responseFdAddList.push_back(pfd);
}

void	Client::addFds()
{
	for (std::vector<PollFd>::iterator it = responseFdAddList.begin();
		it < responseFdAddList.end(); ++it)
	{
		if (std::find(responseFds.begin(),
					responseFds.end(), it->fd) == responseFds.end())
		{
			c->addFd(*it);
			responseFds.push_back(it->fd);
		}
	}
	responseFdAddList.clear();
}
