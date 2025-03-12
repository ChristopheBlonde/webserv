/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 11:59:15 by cblonde           #+#    #+#             */
/*   Updated: 2025/03/12 20:48:34 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Server.hpp>

EXC_FUNC(Server, ServerStartException, "server start failed");

Server::Server(size_t index) :
	index(index)
{
}

Server::~Server()
{
}

#include <string.h>

int	Server::start()
{
	int		opt = 1;
	sockaddr_in	sin;

	socketFd = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (socketFd == -1)
		throw (ServerStartException("cannot create socket"));
	//memset(reinterpret_cast<char *>(&sin), 0, sizeof(sin));
	sin.sin_addr.s_addr = ip;
	sin.sin_family = AF_INET;
	sin.sin_port = port;
	//
	std::cout << sin.sin_port << "\n";
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	sin.sin_family = AF_INET;
	//sin.sin_port = htons(8080);
	std::cout << sin.sin_port << "\n";

	if (setsockopt(socketFd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) == -1
		|| setsockopt(socketFd, SOL_SOCKET, SO_REUSEPORT, &opt, sizeof(opt)) == -1)
		throw(ServerStartException("cannot set socket options"));
	if (bind(socketFd, reinterpret_cast<sockaddr *>(&sin),
				sizeof(sin)) == -1)
		throw (ServerStartException("cannot bind:" + std::string(strerror(errno))));
//	if (listen(_socket, 100) == SOCKET_ERROR)
//		throw (Server::ServerException(
//					std::string("Error: listen: ") + strerror(errno)));
	std::cout << "done\n";
	return (socketFd);
}

//////reuse port, address
//Server::~Server(void)
//{
//	if (this->_socket != -1)
//		if (close(this->_socket) == SOCKET_ERROR)
//			throw (Server::ServerException(
//						std::string("Error: close: ") + strerror(errno)));
//	for (std::vector<pollfd>::iterator it = _fds.begin(); it != _fds.end(); it++)
//	{
//		if (it->fd != -1)
//			if (close(it->fd) == SOCKET_ERROR)
//				throw (Server::ServerException(
//							std::string("Error: close: ") + strerror(errno)));
//	}
//	return ;
//}
//
//Server::Server(Server const &src)
//{
//	*this = src;
//	return ;
//}
//
//Server &Server::operator=(Server const &rhs)
//{
//	if (this != &rhs)
//	{
//		this->_fds = rhs._fds;
//		this->_socket = rhs._socket;
//		this->_port = rhs._port;
//		this->_address = rhs._address;
//	}
//	return (*this);
//}
//
//long int	Server::init(void)
//{
//	sockaddr_in	sin;
//
//	if ((_socket = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0))
//			== INVALID_SOCKET)
//		throw (Server::ServerException(
//					std::string("Error: socket: ") + strerror(errno)));
//	std::memset(reinterpret_cast<char *>(&sin), 0, sizeof(sin));
//	sin.sin_addr.s_addr = htonl(INADDR_ANY);
//	sin.sin_family = AF_INET;
//	sin.sin_port = htons(this->_port);
//	int opt = 1;//without this, error when netcat is not closed properly
//	if (setsockopt(_socket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0)
//	{
//		throw (Server::ServerException(
//					std::string("Error: setsockopt: ") + strerror(errno)));
//	}
//	if (bind(_socket, reinterpret_cast<sockaddr *>(&sin),
//				sizeof(sin)) == SOCKET_ERROR)
//		throw (Server::ServerException(
//					std::string("Error: bind: ") + strerror(errno)));
//	if (listen(_socket, 100) == SOCKET_ERROR)
//		throw (Server::ServerException(
//					std::string("Error: listen: ") + strerror(errno)));
//	return (_socket);
//}
//
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
//	fd.events = POLL_IN;
//	_fds.push_back(fd);
//}
//
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
//
//Server::ServerException::ServerException(std::string str)
//{
//	this->_str = std::string(RED) + str + std::string(RESET);
//	return ;
//}
//
//Server::ServerException::~ServerException(void) throw()
//{
//	return ;
//}
//
//const char *Server::ServerException::getStr(void) const
//{
//	return (this->_str.c_str());
//}
//
//const char *Server::ServerException::what() const throw()
//{
//	return (this->getStr());
//}
//	private:
//		std::vector<pollfd>	_fds;
//		long int		_socket;
//		short int		_port;
//		std::string		_address;
//		Server(void);
//	public:
//		Server(short int port);
//		Server(Server const &src);
//		~Server(void);
//		Server &operator=(Server const &rhs);
//		long int	init(void);
//		void		run(void);
//		void		get_client_maybe(void);
//		class ServerException : public std::exception
//		{
//			private:
//				std::string	_str;
//			public:
//				ServerException(std::string const str);
//				virtual ~ServerException(void) throw();
//				virtual const char *what(void) const throw();
//				const char	*getStr(void) const;
//		};
