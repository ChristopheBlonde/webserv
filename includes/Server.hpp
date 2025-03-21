/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 11:46:41 by cblonde           #+#    #+#             */
/*   Updated: 2025/03/20 17:50:50 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#include <cstring>
#include <unistd.h>
#include "Conf.hpp"
#include "Route.hpp"
#include "ExceptionTemplate.hpp"

class Server : public Conf
{
	EXC_CLASS(ServerStartException);

	static std::map<uint64_t, int>	socketIdMap;

	uint64_t	socketId;
	int		socketFd;

	public:
	Server();
	~Server();
	int	start();
};

#endif
