/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon           <skibidi@ohio.sus>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1833/02/30 06:67:85 by glaguyon          #+#    #+#             */
/*   Updated: 2025/03/25 18:58:45 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <map>
#include <iostream>
#include <sstream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <stdint.h>
#include <netdb.h>
#include "Response.hpp"

#define BUFFER_SIZE 1024
#define MAXLENHEADER 1e11

class Cluster;

enum e_transfer
{
	UNDEFINED,
	LENGTH,
	CHUNKED
};

enum e_mode
{
	HEADERS,
	BODY
};

enum e_chunkmode
{
	CHUNKLEN,
	CHUNKLENEND,
	CHUNKBODY,
	CHUNKBODYEND,
	CHUNKLASTLINE
};

class Client
{
	bool		on;
	int		fd;
	uint32_t	ip;
	uint16_t	port;
	std::string	ipStr;
	std::string	portStr;
	std::string	hostName;
	size_t		bufferSize;

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
	
	std::map<int, Response *>		ress;
	std::map<int, Response *>		files;

	public:
	Client();
	Client(int fd, struct sockaddr_in addr);
	~Client();
	uint64_t	getIp();
	std::string	getIpStr();
	uint16_t	getPort();
	std::string	getPortStr();
	std::string	getHostName();
	size_t		getBufferSize();
	void		init();
	void		handleRequest(Cluster &c);
	void		handleResponse(Cluster &c);
	void	handleFiles(Cluster &c);//
};

#endif // CLIENT_HPP
