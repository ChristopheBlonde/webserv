/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 11:46:41 by cblonde           #+#    #+#             */
/*   Updated: 2025/03/14 18:58:57 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#include <string>
#include <iostream>
#include <map>
#include <unistd.h>
#include <string.h>
#include "Conf.hpp"
#include "Route.hpp"
#include "ExceptionTemplate.hpp"

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
	//TODO add func to connect to client
};

#endif
