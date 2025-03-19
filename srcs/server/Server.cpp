/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 11:59:15 by cblonde           #+#    #+#             */
/*   Updated: 2025/03/19 21:26:50 by cblonde          ###   ########.fr       */
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

//void	Server::run(void)
//{
//	int			check = -1;
//
//	get_client_maybe();
//	if (_fds.size())
//		check = poll(_fds.data(), _fds.size(), 5000);
//	if (check < 0)
//		return ;
//	for (size_t i = 0; i < _fds.size(); i++)
//	{
//		if (_fds[i].fd == -1)
//		{
//			std::cout << "ERrr -1" << std::endl;
//			continue ;
//		}
//		if (_fds[i].revents & POLLERR)
//			throw Server::ServerException(
//					std::string("ERROR: poll: ") + strerror(errno));
//		if (_fds[i].revents & POLLHUP)
//		{
//			close(_fds[i].fd);
//			_fds.erase(_fds.begin() + i--);
//			continue;
//		}
//		std::map<int, Response *>::iterator  it;
//		std::map<int, Response *>::iterator  ite;
//		it = ress.find(_fds[i].fd);
//		ite = files.find(_fds[i].fd);
//		if (it != ress.end())
//		{
//			if (!it->second->handleInOut(_fds[i]))
//			{
//				std::cout << "dans delete" << std::endl;
//				delete it->second;
//				ress.erase(it);
//				//_fds.erase(_fds.begin() + i--);
//			}
//		}
//		else if (ite != files.end())
//		{
//			if (!ite->second->handleInOut(_fds[i]))
//			{
//				close (ite->first);
//				files.erase(ite);
//				_fds.erase(_fds.begin() + i--);
//			}
//		}
//		else
//			handleRequests(_fds[i]);
//		handleFiles();
//	}
//	return ;
//}
}
