/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 11:59:15 by cblonde           #+#    #+#             */
/*   Updated: 2025/03/18 16:23:43 by cblonde          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Server.hpp>

Server::Server(void)
{
	return ;
}

Server::Server(short int port) : _port(port)
{
	this->_socket = -1;
	this->_address = -1;
	return ;
}

Server::~Server(void)
{
	if (this->_socket != -1)
		if (close(this->_socket) == SOCKET_ERROR)
			throw (Server::ServerException(
						std::string("Error: close: ") + strerror(errno)));
	for (std::vector<pollfd>::iterator it = _fds.begin(); it != _fds.end(); it++)
	{
		if (it->fd != -1)
			if (close(it->fd) == SOCKET_ERROR)
				throw (Server::ServerException(
							std::string("Error: close: ") + strerror(errno)));
	}
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

	if ((_socket = socket(AF_INET, SOCK_STREAM /*| SOCK_NONBLOCK*/, 0))
			== INVALID_SOCKET)
		throw (Server::ServerException(
					std::string("Error: socket: ") + strerror(errno)));
	fcntl(_socket, F_SETFL, O_NONBLOCK);
	std::memset(reinterpret_cast<char *>(&sin), 0, sizeof(sin));
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(this->_port);
	int opt = 1;//without this, error when netcat is not closed properly
	if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
	{
		throw (Server::ServerException(
					std::string("Error: setsockopt: ") + strerror(errno)));
	}
	if (bind(_socket, reinterpret_cast<sockaddr *>(&sin),
				sizeof(sin)) == SOCKET_ERROR)
		throw (Server::ServerException(
					std::string("Error: bind: ") + strerror(errno)));
	if (listen(_socket, 100) == SOCKET_ERROR)
		throw (Server::ServerException(
					std::string("Error: listen: ") + strerror(errno)));
	return (_socket);
}

void	Server::get_client_maybe(void)
{
	sockaddr_in	sin;
	socklen_t	size_sin = sizeof(sin);
	long int	client = -1;
	pollfd		fd;

	std::memset(reinterpret_cast<char *>(&sin), 0, sizeof(sin));
	client = accept(_socket, reinterpret_cast<sockaddr *>(&sin),
			&size_sin);
	if (client == -1)
		return ;
	fd.fd = client;
	fd.events = POLL_IN | POLL_OUT;
	_fds.push_back(fd);
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

void	Server::handleRequests(struct pollfd &fd)
{
	int		readByte;
	char	buffer[BUFFER_SIZE];

	if (!(fd.revents & POLL_IN))
		return ;
	readByte = recv(fd.fd, buffer, BUFFER_SIZE - 1, 0);
	if (readByte > 0)
		buffer[readByte] = '\0';
	requests[fd.fd] += buffer;
	if (checkEndOfFile(requests[fd.fd]))
	{
		std::cout << CYAN << requests[fd.fd] << std::endl << RESET;
		Requests req(requests[fd.fd]);
		Response *res = new Response(req);
		res->setSocket(fd.fd);
		std::cout << GREEN << "new Request create with Socket: "
			<< res->getSocket() << RESET << std::endl;
		ress[fd.fd] = res;
		requests.erase(fd.fd);	
	}
}

void	Server::handleFiles(void)
{
	struct pollfd fd;

	for (std::map<int, Response *>::iterator it = ress.begin();
			it != ress.end(); it++)
	{
		fd.fd = it->second->getFileFd();
		if (fd.fd < 0 || (files.find(fd.fd) != files.end()))
			continue ;
		fd.events = POLL_IN;
		fd.revents = 0;
		_fds.push_back(fd);
		files.insert(std::make_pair(fd.fd, it->second));
	}
}

void	Server::run(void)
{
	int			check = -1;

	get_client_maybe();
	if (_fds.size())
		check = poll(_fds.data(), _fds.size(), 5000);
	if (check < 0)
		return ;
	for (size_t i = 0; i < _fds.size(); i++)
	{
		if (_fds[i].fd == -1)
		{
			std::cout << "ERrr -1" << std::endl;
			continue ;
		}
		if (_fds[i].revents & POLL_ERR)
			throw Server::ServerException(
					std::string("ERROR: poll: ") + strerror(errno));
		if (_fds[i].revents & POLLHUP)
		{
			close(_fds[i].fd);
			_fds.erase(_fds.begin() + i--);
			continue;
		}
		std::map<int, Response *>::iterator  it;
		std::map<int, Response *>::iterator  ite;
		it = ress.find(_fds[i].fd);
		ite = files.find(_fds[i].fd);
		if (it != ress.end())
		{
			if (!it->second->handleInOut(_fds[i]))
			{
				std::cout << "dans delete" << std::endl;
				delete it->second;
				ress.erase(it);
				_fds.erase(_fds.begin() + i--);
			}
		}
		else if (ite != files.end())
		{
			if (!ite->second->handleInOut(_fds[i]))
			{
				close (ite->first);
				files.erase(ite);
				_fds.erase(_fds.begin() + i--);
			}
		}
		else
			handleRequests(_fds[i]);
		handleFiles();
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
