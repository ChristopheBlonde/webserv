/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon           <skibidi@ohio.sus>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1833/02/30 06:67:85 by glaguyon          #+#    #+#             */
/*   Updated: 2025/03/21 01:35:33 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLUSTER_HPP
# define CLUSTER_HPP

#include <vector>
#include <string>
#include <iostream>
#include "Client.hpp"
#include "PollFd.hpp"
#include "Server.hpp"
#include "ConfParser.hpp"
#include "Response.hpp"
#include "Requests.hpp"

#define POLL_TIMEOUT 1000

class Cluster
{
	std::vector<Server>			servers;
	std::vector<PollFd>			fds;
	std::map<int, std::vector<Server *> >	serverFds;
	std::map<int, Client>			clients;
	std::map<int, int>			clientFdToServFd;
	std::vector<int>			clientCloseList;
	public:
	std::map<int, Response *>		ress;//
	std::map<int, Response *>		files;//
	private:
	PollFd	&getPollFd(int fd);
	void	destroyClients();
	void	addClients();
	void	handleFiles(void);//

	public:
	static const std::string	defaultRoot;
	static const std::string	allowedMethods[];
	static const size_t		allowedMethodsSize;
	
	Cluster(const std::string &filename);
	~Cluster();
	Server			*addServer();
	std::vector<Server>	&getServers();
	void			startServers();
	Server			&getServer(int fd, const std::string &host);
	Route			&getRoute(Route &r, const std::string &path);
	void			closeClient(int fd);
	void			run();
};

#endif // CLUSTER_HPP
