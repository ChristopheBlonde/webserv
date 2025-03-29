/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon           <skibidi@ohio.sus>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1833/02/30 06:67:85 by glaguyon          #+#    #+#             */
/*   Updated: 2025/03/29 17:08:18 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <vector>
#include <queue>
#include <map>
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <stdint.h>
#include <netdb.h>
#include "Response.hpp"
#include "Receiver.hpp"

class Cluster;

class Client : public Receiver
{
	bool		on;

	Cluster		*c;
	int		fd;
	uint32_t	ip;
	uint16_t	port;
	std::string	ipStr;
	std::string	portStr;
	std::string	hostName;

	void		removeFds();
	void		addFds();
	
	std::queue<Response *>		responses;
	std::vector<int>		responseFds;
	std::vector<int>		responseFdRemoveList;
	std::vector<PollFd>		responseFdAddList;

	public:
	Client();
	Client(int fd, struct sockaddr_in addr, Cluster *c);
	~Client();
	uint64_t	getIp();
	std::string	getIpStr();
	uint16_t	getPort();
	std::string	getPortStr();
	std::string	getHostName();
	void		init();
	void		handleRequest();
	void		handleResponse();
	void		addResponseFd(PollFd);
};

#endif // CLIENT_HPP
