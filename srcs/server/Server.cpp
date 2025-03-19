/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 11:59:15 by cblonde           #+#    #+#             */
/*   Updated: 2025/03/19 22:34:11 by cblonde          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Server.hpp>

EXC_FUNC(Server, ServerStartException, "server start failed");

std::map<uint64_t, int>	Server::socketIdMap;

Server::Server(size_t index) :
	index(index)
{
	this->index = index;
}

int	Server::start()
{
	int		opt = 1;
	sockaddr_in	sin;

	socketId = (static_cast<uint64_t>(ip) << 32) | static_cast<uint64_t>(port);
	if (socketIdMap.find(socketId) != socketIdMap.end())
	{
		socketFd = socketIdMap[socketId];
		return socketFd;
	}
	socketFd = socket(AF_INET, SOCK_STREAM /*| SOCK_NONBLOCK*/, 0);
	socketIdMap[socketId] = socketFd;
	if (socketFd == -1)
		throw (ServerStartException("cannot create socket"));
	fcntl(socketFd, F_SETFL, O_NONBLOCK);
	sin.sin_addr.s_addr = ip;
	sin.sin_family = AF_INET;
	sin.sin_port = port;
	if (setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1
		|| setsockopt(socketFd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) == -1)
		throw (ServerStartException("cannot set socket options: "
			+ std::string(strerror(errno))));
	if (bind(socketFd, reinterpret_cast<sockaddr *>(&sin),
				sizeof(sin)) == -1)
		throw (ServerStartException("cannot bind: " + std::string(strerror(errno))));
	if (listen(socketFd, 100) == -1)
		throw (ServerStartException("cannot listen: " + std::string(strerror(errno))));
	return socketFd;
}

Server::~Server()
{
	std::map<uint64_t, int>::iterator	it = socketIdMap.find(socketId);

	if (it != socketIdMap.end())
	{
		close(socketIdMap[socketId]);
		socketIdMap.erase(it);
	}
}
