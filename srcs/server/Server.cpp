/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 11:59:15 by cblonde           #+#    #+#             */
/*   Updated: 2025/03/14 15:19:05 by cblonde          ###   ########.fr       */
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

void	Server::run(void)
{
	int			check = -1;
	char		buffer[5];
	int			readByte;
	
	get_client_maybe();
	if (_fds.size())
		check = poll(_fds.data(), _fds.size(), 5000);
	if (check < 0)
		return ;
	for (std::map<int, Response *>::iterator it = ress.begin();
			it !=  ress.end(); it++)
	{
		struct pollfd tmp = it->second->getFileStatus();
		if (files.find(tmp.fd) == files.end() && tmp.fd >= 0)
		{
			std::cout << GREEN << "In add file to poll" << std::endl << RESET;
			_fds.push_back(tmp);
			if (tmp.events & (POLLIN))
				files[tmp.fd] = "";
		}
	}
	for (size_t i = 0; i < _fds.size(); i++)
	{
		memset(&buffer, 0, 5);
		if (_fds[i].fd == -1)
			continue ;
		if (_fds[i].revents & (POLL_ERR | POLL_HUP))
			throw Server::ServerException(
					std::string("ERROR: poll: ") + strerror(errno));
		if (_fds[i].revents & (POLL_IN))
		{
			readByte = recv(_fds[i].fd, buffer, 4, 0);
			if (readByte != 0)
				buffer[readByte] = '\0';
			/* add to request map or file map */
			
			/* find if request already exist or is file */
			if (files.find(_fds[i].fd) != files.end())
			{
				readByte = read(_fds[i].fd, buffer, 4);
				buffer[readByte] = '\0';
				/* if file update string */
					/* if read finish close pollfd and exec response */
				if (readByte == 0)
				{
					struct pollfd tmp;
					tmp.fd = -1;
					tmp.events = 0;
					tmp.revents = 0;
					for (std::map<int, Response *>::iterator it = ress.begin();
							it != ress.end(); it++)
					ress[_fds[i].fd]->setFileContent(files[_fds[i].fd]);
					ress[_fds[i].fd]->setFileStatus(tmp);
					close(_fds[i].fd);
					ress[_fds[i].fd]->createResponse();
					_fds.erase(_fds.begin() + i--);
					continue ;
				}
					/* if file not finish add string and continue */
					files[_fds[i].fd] += buffer;
					continue ;
			}
			/* if request */
			if (requests.find(_fds[i].fd) != requests.end())
			{
				/* if read finish create request and delete in map */
				requests[_fds[i].fd] += buffer;
				if (checkEndOfFile(requests[_fds[i].fd]))
				{
					Requests req(requests[_fds[i].fd]);
					Response *res = new Response(req);
					res->setSocket(_fds[i].fd);
					ress[res->getFileStatus().fd] = res;
					requests.erase(_fds[i].fd);	
				}
			}
			else
			{
				/* add new request in requests map */
				requests[_fds[i].fd] = buffer;
				if (checkEndOfFile(requests[_fds[i].fd]))
				{
					Requests req(requests[_fds[i].fd]);
					Response *res = new Response(req);
					ress[res->getFileStatus().fd] = res;
					requests.erase(_fds[i].fd);	
				}
			}		
			if (readByte == 0)
			{
				std::cout << "client " << _fds[i].fd << " disconnected\n";
				_fds.erase(_fds.begin() + i--);
				break;
			}
			if (_fds[i].revents & (POLL_OUT))
			{
				std::cout << RED << "pollout:  " << _fds[i].fd << std::endl
					<< RESET;
				for (std::map<int, Response *>::iterator it = ress.begin();
						it != ress.end(); it++)
				{
					Response *tmp = it->second;
					if (tmp->getSocket() == _fds[i].fd)
					{
						std::cout << RED << "trouve une reponse" << std::endl
							<< RESET;
						std::string	file = tmp->getResponse();
						send(_fds[i].fd, file.c_str(), 4, 0);
						if (file.size() > 4)
							tmp->setResponse(file.substr(4));
						else
						{
							delete (tmp);
							ress.erase(it);
						}
					}
				}
			}
							
							
				
			//Requests	test1((std::string(request)));
			//Response	test2(test1);
			//test2.createResponse();

			//send(_fds[i].fd, test2.getResponse().c_str(), test2.getResSize(), 0);
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
