/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon           <skibidi@ohio.sus>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1833/02/30 06:67:85 by glaguyon          #+#    #+#             */
/*   Updated: 2025/03/21 00:22:05 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>

#define BUFFER_SIZE 1024
#define MAXLENHEADER 1e11

class Cluster;

enum
{
	UNDEFINED,
	LENGTH,
	CHUNKED
} e_transfer;

enum
{
	HEADERS,
	BODY
} e_mode;

enum
{
	CHUNKLEN,
	CHUNKLENEND,
	CHUNKBODY,
	CHUNKBODYEND
} e_chunkmode;

class Client
{
	bool		on;
	int		fd;

	size_t		readSize;
	std::string	currRequest;
	std::string	currRequestRaw;
	char		buffer[BUFFER_SIZE];
	int		transferType;
	int		mode;
	int		chunkMode;
	
	void		handleRequestHeaders(Cluster &c);
	bool		handleRequestBodyChunked(Cluster &c);
	bool		handleRequestBodyLength(Cluster &c);
	void		resetRequest();
	int		getTransferType(Cluster &c);

	public:
	Client();
	Client(int fd);
	~Client();
	void		init();
	void		handleRequest(Cluster &c);
	void		handleResponse(int fd);
};

#endif // CLIENT_HPP
