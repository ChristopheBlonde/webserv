/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 11:46:41 by cblonde           #+#    #+#             */
/*   Updated: 2025/03/19 18:45:37 by cblonde          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

	//	std::vector<pollfd>	_fds;
	//	long int		_socket;
	//	short int		_port;
	//	std::string		_address;	
	//	std::map<int, std::string>	requests;
	//	//std::map<int, std::string>	files;
	//	std::map<int, Response *>	ress;
	//	std::map<int, Response *> files;
	//	Server(void);
	//	void	handleRequests(struct pollfd &fd);
	//	void	handleFiles(void);
#include <webserv.hpp>
#include <Conf.hpp>
#include <Route.hpp>
#include <ExceptionTemplate.hpp>

class Server : public Conf
{
	EXC_CLASS(ServerStartException);

	static std::map<uint64_t, int>	socketIdMap;

	size_t		index;
	uint64_t	socketId;
	int		socketFd;

	public:
	Server(size_t index);
	~Server();
	int	start();
};

#endif
