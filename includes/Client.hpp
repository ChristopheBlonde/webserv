/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon           <skibidi@ohio.sus>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1833/02/30 06:67:85 by glaguyon          #+#    #+#             */
/*   Updated: 2025/03/22 18:52:08 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CLIENT_HPP
# define CLIENT_HPP

#include <map>
#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include "Response.hpp"

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
	CHUNKBODYEND,
	CHUNKLASTLINE
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
	
	std::map<int, Response *>		ress;
	std::map<int, Response *>		files;

	public:
	Client();
	Client(int fd);
	~Client();
	void		init();
	void		handleRequest(Cluster &c);
	void		handleResponse(Cluster &c);
	void	handleFiles(Cluster &c);//
};

#endif // CLIENT_HPP
