/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 11:59:15 by cblonde           #+#    #+#             */
/*   Updated: 2025/03/19 18:52:15 by cblonde          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Server.hpp>

EXC_FUNC(Server, ServerStartException, "server start failed");

std::map<uint64_t, int>	Server::socketIdMap;

Server::Server(size_t index) :
	index(index)
{
}

int	Server::start()
{
	int		opt = 1;
	sockaddr_in	sin;

//<<<<<<< HEAD
//	if ((_socket = socket(AF_INET, SOCK_STREAM /*| SOCK_NONBLOCK*/, 0))
//			== INVALID_SOCKET)
//		throw (Server::ServerException(
//					std::string("Error: socket: ") + strerror(errno)));
//	fcntl(_socket, F_SETFL, O_NONBLOCK);
//	std::memset(reinterpret_cast<char *>(&sin), 0, sizeof(sin));
//	sin.sin_addr.s_addr = htonl(INADDR_ANY);
//=======
	socketId = (static_cast<uint64_t>(ip) << 32) | static_cast<uint64_t>(port);
	if (socketIdMap.find(socketId) != socketIdMap.end())
	{
		socketFd = socketIdMap[socketId];
		return socketFd;
	}
	socketFd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	socketIdMap[socketId] = socketFd;
	if (socketFd == -1)
		throw (ServerStartException("cannot create socket"));
	sin.sin_addr.s_addr = ip;
//>>>>>>> origin/start_servers
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
//<<<<<<< HEAD
//	if (bind(_socket, reinterpret_cast<sockaddr *>(&sin),
//				sizeof(sin)) == SOCKET_ERROR)
//		throw (Server::ServerException(
//					std::string("Error: bind: ") + strerror(errno)));
//	if (listen(_socket, 100) == SOCKET_ERROR)
//		throw (Server::ServerException(
//					std::string("Error: listen: ") + strerror(errno)));
//	return (_socket);
//}

//void	Server::get_client_maybe(void)
//{
//	sockaddr_in	sin;
//	socklen_t	size_sin = sizeof(sin);
//	long int	client = -1;
//	pollfd		fd;
//
//	std::memset(reinterpret_cast<char *>(&sin), 0, sizeof(sin));
//	client = accept(_socket, reinterpret_cast<sockaddr *>(&sin),
//			&size_sin);
//	if (client == -1)
//		return ;
//	fd.fd = client;
//	fd.events = POLLIN | POLLOUT;
//	_fds.push_back(fd);
//}

//bool	checkEndOfFile(std::string str)
//{
//	size_t		index;
//	std::string	headers;
//	std::string body;
//	size_t		len;
//	size_t		start;
//	size_t		end;
//	size_t		body_start;
//
//	index = str.find("\r\n\r\n");
//	if (index == std::string::npos)
//		return (false);
//	body_start = index + 4;
//	headers = str.substr(0, index + 4);
//	index = headers.find("Content-Length: ");
//	if (index != std::string::npos)
//	{
//		start = index + 16;
//		end	= headers.find("\r\n", start);
//		if (end == std::string::npos)
//			return (false);
//		len = std::atoi(headers.substr(start, end - start).c_str());
//		if (str.size() >= body_start + len)
//			return (true);
//		else
//			return (false);
//	}
//	index = headers.find("Transfer-Encoding: chunked");
//	if (index == std::string::npos)
//		return (true);
//	index = str.find("0\r\n\r\n");
//	if (index == std::string::npos)
//		return (false);
//	else
//		return (true);
//}

//void	Server::handleRequests(struct pollfd &fd)
//{
//	int		readByte;
//	char	buffer[BUFFER_SIZE];
//
//	if (!(fd.revents & POLLIN))
//		return ;
//	readByte = recv(fd.fd, buffer, BUFFER_SIZE - 1, 0);
//	if (readByte > 0)
//		buffer[readByte] = '\0';
//	requests[fd.fd] += buffer;
//	if (checkEndOfFile(requests[fd.fd]))
//	{
//		std::cout << CYAN << requests[fd.fd] << std::endl << RESET;
//		Requests req(requests[fd.fd]);
//		Response *res = new Response(req);
//		res->setSocket(fd.fd);
//		std::cout << GREEN << "new Request create with Socket: "
//			<< res->getSocket() << RESET << std::endl;
//		ress[fd.fd] = res;
//		requests.erase(fd.fd);	
//	}
//}

//void	Server::handleFiles(void)
//{
//	struct pollfd fd;
//
//	for (std::map<int, Response *>::iterator it = ress.begin();
//			it != ress.end(); it++)
//	{
//		fd.fd = it->second->getFileFd();
//		if (fd.fd < 0 || (files.find(fd.fd) != files.end()))
//			continue ;
//		fd.events = POLLIN;
//		fd.revents = 0;
//		_fds.push_back(fd);
//		files.insert(std::make_pair(fd.fd, it->second));
//	}
//}

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
//
//Server::ServerException::ServerException(std::string str)
//{
//	this->_str = std::string(RED) + str + std::string(RESET);
//	return ;
//}

//Server::ServerException::~ServerException(void) throw()
//{
//	return ;
//}

//const char *Server::ServerException::getStr(void) const
//{
//	return (this->_str.c_str());
//}

//const char *Server::ServerException::what() const throw()
//{
//	return (this->getStr());
//=======
//>>>>>>> origin/start_servers
}
