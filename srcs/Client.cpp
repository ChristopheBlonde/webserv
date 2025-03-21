/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon           <skibidi@ohio.sus>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1833/02/30 06:67:85 by glaguyon          #+#    #+#             */
/*   Updated: 2025/03/21 01:31:47 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Cluster.hpp"

Client::Client() :
	on(false),
	fd(-1)
{
	resetRequest();
}

Client::Client(int fd) :
	on(false),
	fd(fd)
{
	resetRequest();
}

Client::~Client()
{
	if (!on)
		return;
	close(fd);
}

void	Client::init()
{
	on = true;
}

void	Client::resetRequest()
{
	readSize = 1;
	currRequest = "";
	currRequestRaw = "";
	transferType = UNDEFINED;
	mode = HEADERS;
	chunkMode = CHUNKLEN;
}

int	Client::getTransferType(Cluster &c)
{
	size_t	transferHeader = currRequest.find("transfer-encoding:");

	if (transferHeader != std::string::npos)
	{
		transferHeader += std::strlen("transfer-encoding:"); 
		while (currRequest[transferHeader] == ' '
			|| currRequest[transferHeader] == '\t')
			++transferHeader;
		if (currRequest.find("chunked", transferHeader) == transferHeader)
			return CHUNKED;
	}
	transferHeader = currRequest.find("content-length:");
	if (transferHeader == std::string::npos)
		return UNDEFINED;
	if (currRequest.find("content-length:", transferHeader) != std::string::npos)
	{
		c.closeClient(fd);
		readSize = 1;
		return LENGTH;
	}

	char	*num = &currRequest[transferHeader + std::strlen("content-length:")];
	readSize = 0;

	while (*num == ' ' || *num == '\t')
		++num;
	while (*num >= '0' && *num <= '9' && readSize < MAXLENHEADER)
		readSize = readSize * 10 + *num++ - '0';
	while (*num == ' ' || *num == '\t')
		++num;
	if (num[0] != '\r' || num[1] != '\n')
	{
		c.closeClient(fd);
		readSize = 1;
		return LENGTH;
	}
	return LENGTH;
}

void	Client::handleRequestHeaders(Cluster &c)
{
	ssize_t	readByte;

	readByte = recv(fd, buffer, 1, 0);
	if (readByte <= 0)
	{
		c.closeClient(fd);
		return ;
	}
	else
		buffer[readByte] = '\0';

	std::string	bufferLow = buffer;
	
	for (std::string::iterator it = bufferLow.begin(); it < bufferLow.end(); ++it)
		*it = std::tolower(*it);
	currRequest += bufferLow;
	currRequestRaw += buffer;
	if (currRequest.size() < 4
		|| currRequest.compare(currRequest.size() - 4, 4, "\r\n\r\n") != 0)
		return ;
	transferType = getTransferType(c);
	mode = BODY;
}

bool	Client::handleRequestBodyChunked(Cluster &c)
{
	ssize_t	readByte;

	if (readSize > BUFFER_SIZE - 1)
		readByte = recv(fd, buffer, BUFFER_SIZE - 1, 0);
	else
		readByte = recv(fd, buffer, readSize, 0);
	if (readByte <= 0)
	{
		c.closeClient(fd);
		return false;
	}
	else
		buffer[readByte] = '\0';

	switch (chunkMode)
	{
	case CHUNKLENEND:
		if (*buffer != '\n')
		{
			c.closeClient(fd);
			return false;
		}
		readSize = 0;
		for (std::string::iterator it = currRequest.begin(); it <= currRequest.end(); ++it)
		{
			if (*it >= '0' && *it <= '9')
				readSize = readSize * 16 + *it - '0';
			else if (*it >= 'a' && *it <= 'f')
				readSize = readSize * 16 + *it - 'a';
			else
				break;
			if (readSize > MAXLENHEADER)
			{
				c.closeClient(fd);
				return false;
			}
		}
		currRequest = "";
		if (readSize == 0)
		{
			chunkMode = CHUNKLASTLINE;
			readSize = 1;
		}
		else
			chunkMode = CHUNKBODY;
		break;

	case CHUNKLEN:
		if (*buffer == '\r')
			chunkMode = CHUNKLENEND;
		else
			currRequest += std::tolower(*buffer);
		break;

	case CHUNKBODY:
		readSize -= readByte;
		currRequestRaw += buffer;
		if (readSize == 0)
		{
			chunkMode = CHUNKBODYEND;
			readSize = 2;
		}
		break;

	case CHUNKBODYEND:
		readSize -= readByte;
		currRequest += buffer;
		if (readSize == 0)
		{
			if (currRequest != "\r\n")
			{
				c.closeClient(fd);
				return false;
			}
			chunkMode = CHUNKLEN;
			readSize = 1;
			currRequest = "";
		}
		break;
	
	case CHUNKLASTLINE:
		currRequest += buffer;
		if (currRequest.size() >= 4
			&& currRequest.compare(currRequest.size() - 4, 4, "\r\n\r\n") == 0)
			return true;
		break;
	}
	return false;
}

bool	Client::handleRequestBodyLength(Cluster &c)
{
	ssize_t	readByte;

	if (readSize > BUFFER_SIZE - 1)
		readByte = recv(fd, buffer, BUFFER_SIZE - 1, 0);
	else
		readByte = recv(fd, buffer, readSize, 0);
	if (readByte <= 0)
	{
		c.closeClient(fd);
		return false;
	}
	buffer[readByte] = '\0';
	readSize -= readByte;
	currRequestRaw += buffer;
	if (readSize == 0)
		return true;
	return false;
}

void	Client::handleRequest(Cluster &c)
{
	if (mode == HEADERS)
		handleRequestHeaders(c);
	if ((mode == BODY && transferType == UNDEFINED)
		|| (mode == BODY && transferType == CHUNKED && handleRequestBodyChunked(c))
		|| (mode == BODY && transferType == LENGTH && handleRequestBodyLength(c)))
	{
//		std::cout << CYAN << requests[fd.fd] << std::endl << RESET;
//		Requests req(requests[fd.fd]);
//		Response *res = new Response(req);
//		res->setSocket(fd.fd);
//		std::cout << GREEN << "new Request create with Socket: "
//			<< res->getSocket() << RESET << std::endl;
//		ress[fd.fd] = res;
//		requests.erase(fd.fd);
//
		resetRequest();
	}
}

void	Client::handleResponse(int fd)
{
}
