/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 11:59:15 by cblonde           #+#    #+#             */
/*   Updated: 2025/02/20 14:52:17 by cblonde          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Server.hpp>

Server::Server(void)
{
	return ;
}

Server::Server(short int port) : _port(port)
{
	return ;
}

Server::~Server(void)
{
	return ;
}

Server::Server(Server const &src)
{
	*this = src;
	return ;
}

Server &Server::operator=(Server const &rhs)
{
	if (this != &rhs)
	{
		this->_fds = rhs._fds;
		this->_socket = rhs._socket;
		this->_port = rhs._port;
		this->_address = rhs._address;
	}
	return (*this);
}

long int	Server::init(void)
{
	sockaddr_in	sin;

	if ((_socket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 1))
			== INVALID_SOCKET)
		throw (Server::ServerException(
					std::string("Error: socket: ") + strerror(errno)));
	std::memset(reinterpret_cast<char *>(&sin), 0, sizeof(sin));
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(this->_port);
	if (bind(_socket, reinterpret_cast<sockaddr *>(&sin),
				sizeof(sin)) == SOCKET_ERROR)
		throw (Server::ServerException(
					std::string("Error: bind: ") + strerror(errno)));
	if (listen(_socket, 100) == SOCKET_ERROR)
		throw (Server::ServerException(
					std::string("Error: listen: ") + strerror(errno)));
	return (_socket);
}

void	Server::run(void)
{
	long int	client = -1;
	sockaddr_in	sin;
	socklen_t	size_sin = sizeof(sin);
	pollfd		fd;
	int			check;
	char		request[1024];
	int			read;

	std::memset(reinterpret_cast<char *>(&sin), 0, sizeof(sin));
	client = accept(_socket, reinterpret_cast<sockaddr *>(&sin),
			&size_sin);
	if (client == -1)
		return ;
	fd.fd = client;
	fd.events = POLL_IN;
	_fds.push_back(fd);
	check = poll(_fds.data(), _fds.size(), -1);
	if (check < 0)
		return ;
	for (size_t i = 0; i < _fds.size(); i++)
	{
		if (_fds[i].fd == -1)
			continue ;
		if (_fds[i].revents & (POLL_ERR | POLL_HUP))
			throw Server::ServerException(
					std::string("ERROR: poll: ") + strerror(errno));
		if (_fds[i].revents & (POLL_IN))
		{
			read = recv(_fds[i].fd, request, 1023, 0);
			request[read] = '\0';
			std::cout << CYAN << request << RESET << std::endl;
		}
	}
	return ;
}

Server::ServerException::ServerException(std::string str)
{
	this->_str = std::string(RED) + str + std::string(RESET);
	return ;
}

Server::ServerException::~ServerException(void) throw()
{
	return ;
}

const char *Server::ServerException::getStr(void) const
{
	return (this->_str.c_str());
}

const char *Server::ServerException::what() const throw()
{
	return (this->getStr());
}
