/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon           <skibidi@ohio.sus>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1833/02/30 06:67:85 by glaguyon          #+#    #+#             */
/*   Updated: 2025/04/08 17:08:06 by glaguyon         ###   ########.fr       */
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
	std::vector<int>			fdRemoveList;

	std::map<std::string, std::string>	sessionMap;

	PollFd	&getPollFd(int fd);
	void	destroyClients();
	void	addClients();
	void	deleteFds();

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
	short			getRevents(int fd);
	void			closeClient(int fd);
	void			addFd(PollFd pfd);
	void			removeFd(int fd);

	void			removeSession(std::string id);
	std::string		getSession(std::string id);
	void			editSession(std::string key, std::string value);
	void			editSession(std::pair<std::string, std::string>);

	void			run();
};

#endif // CLUSTER_HPP
