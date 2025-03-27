/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 11:59:15 by cblonde           #+#    #+#             */
/*   Updated: 2025/03/26 15:50:10 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Server.hpp"
#include <fcntl.h>

EXC_FUNC(Server, ServerStartException, "server start failed");

std::map<uint64_t, int>	Server::socketIdMap;

Server::Server()
{
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
	#ifdef __linux__
		socketFd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	#else
		socketFd = socket(AF_INET, SOCK_STREAM, 0);
	#endif
	socketIdMap[socketId] = socketFd;
	if (socketFd == -1)
		throw (ServerStartException("cannot create socket"));
	#ifndef __linux__
		fcntl(socketFd, F_SETFL, O_NONBLOCK);
	#endif
	sin.sin_addr.s_addr = ip;
	sin.sin_family = AF_INET;
	sin.sin_port = port;
	if (setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1
		|| setsockopt(socketFd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) == -1)
		throw (ServerStartException("cannot set socket options: "
			+ std::string(std::strerror(errno))));
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
