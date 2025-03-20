/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon           <skibidi@ohio.sus>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1833/02/30 06:67:85 by glaguyon          #+#    #+#             */
/*   Updated: 2025/03/20 22:05:47 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>

enum
{
	UNDEFINED,
	LENGTH,
	CHUNKED
} e_transfer;

class Client
{
	bool		on;
	int		fd;
	std::string	currRequest;
	std::string	currRequestRaw;
	size_t		index;
	int		transferType;
	ssize_t		expectedLen;

	bool		checkEndRequest(ssize_t readByte);
	void		resetRequest();

	public:
	Client();
	Client(int fd);
	~Client();
	void		init();
	void		handleRequest();
	void		handleResponse(int fd);
};

#endif // CLIENT_HPP
