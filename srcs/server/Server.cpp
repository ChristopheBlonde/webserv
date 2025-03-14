/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 11:59:15 by cblonde           #+#    #+#             */
/*   Updated: 2025/03/14 18:32:17 by glaguyon         ###   ########.fr       */
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
