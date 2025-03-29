/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Receiver.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon           <skibidi@ohio.sus>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1833/02/30 06:67:85 by glaguyon          #+#    #+#             */
/*   Updated: 2025/03/29 17:21:49 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RECEIVER_HPP
# define RECEIVER_HPP

#include <iostream>
#include <string>
#include <cstring>
#include <unistd.h>
#include <stdint.h>
#include <netdb.h>
#define MAXLENHEADER 1e11
#define BUFFER_SIZE 1024

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

class Receiver
{
	protected:
	size_t		readSize;
	std::string	currRequest;
	std::string	currRequestRaw;
	char		buffer[BUFFER_SIZE];
	int		transferType;
	int		mode;
	int		chunkMode;
	
	void		handleRequestHeaders(int fd);
	bool		handleRequestBodyChunked(int fd);
	bool		handleRequestBodyLength(int fd);
	void		resetRequest();
	int		getTransferType();

	public:
	Receiver();
	~Receiver();
	bool		receiveRequest(int fd);
};

#endif // RECEIVER_HPP
