/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 11:46:41 by cblonde           #+#    #+#             */
/*   Updated: 2025/03/12 20:07:56 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#include <string>
#include <iostream>
#include "Conf.hpp"
#include "ExceptionTemplate.hpp"

class Server : public Conf
{
	EXC_CLASS(ServerStartException);

	size_t	index;
	int	socketFd;

	public:
	Server(size_t index);
	~Server();
	int	start();
};

#endif
