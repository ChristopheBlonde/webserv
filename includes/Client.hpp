/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon           <skibidi@ohio.sus>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1833/02/30 06:67:85 by glaguyon          #+#    #+#             */
/*   Updated: 2025/03/20 18:30:16 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <iostream>
#include <string>
#include <unistd.h>
#include "Requests.hpp"

class Client
{
	bool		on;
	int		fd;
	std::string	currRequest;

	public:
	Client();
	Client(int fd);
	~Client();
	void		init();
	void		requestAppend(const std::string &s);
	Requests	getRequest();
};

#endif // CLIENT_HPP
