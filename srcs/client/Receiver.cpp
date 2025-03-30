/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Receiver.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon           <skibidi@ohio.sus>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1833/02/30 06:67:85 by glaguyon          #+#    #+#             */
/*   Updated: 2025/03/30 20:04:52 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Receiver.hpp"

Receiver::Receiver()
{
	resetRequest();
}

Receiver::~Receiver()
{
}

void	Receiver::resetRequest()
{
	readSize = 1;
	currRequest = "";
	currRequestRaw = "";
	transferType = UNDEFINED;
	mode = HEADERS;
	chunkMode = CHUNKLEN;
}

int	Receiver::getTransferType()
{
	size_t	transferHeader = currRequest.find("transfer-encoding:");

	if (transferHeader != std::string::npos)
	{
		transferHeader += std::strlen("transfer-encoding:"); 
		while (currRequest[transferHeader] == ' '
			|| currRequest[transferHeader] == '\t')
			++transferHeader;
		size_t	end = currRequest.find("chunked", transferHeader);
		if (end == transferHeader)
		{
			end += std::strlen("chunked");
			while (currRequest[end] == ' ' || currRequest[end] == '\t')
				++end;
			if (currRequest[end] == '\r' && currRequest[end + 1] == '\n')
				return CHUNKED;
		}
	}
	transferHeader = currRequest.find("content-length:");
	if (transferHeader == std::string::npos)
		return UNDEFINED;
	if (currRequest.find("content-length:", transferHeader + 1) != std::string::npos)
		throw std::runtime_error("close the client");

	char	*num = &currRequest[transferHeader + std::strlen("content-length:")];
	readSize = 0;

	while (*num == ' ' || *num == '\t')
		++num;
	while (*num >= '0' && *num <= '9' && readSize < MAXLENHEADER)
		readSize = readSize * 10 + *num++ - '0';
	while (*num == ' ' || *num == '\t')
		++num;
	if (num[0] != '\r' || num[1] != '\n')
		throw std::runtime_error("close the client");
	return LENGTH;
}

void	Receiver::handleRequestHeaders(int fd)
{
	ssize_t	readByte;

	readByte = recv(fd, buffer, 1, 0);
	if (readByte <= 0)
		throw std::runtime_error("close the client");
	else
		buffer[readByte] = '\0';

	std::string	bufferLow(buffer);
	
	for (std::string::iterator it = bufferLow.begin(); it < bufferLow.end(); ++it)
		*it = std::tolower(*it);
	currRequest.append(bufferLow.data(), readByte);
	currRequestRaw.append(buffer, readByte);
	if (currRequest.size() < 4
		|| currRequest.compare(currRequest.size() - 4, 4, "\r\n\r\n") != 0)
		return ;
	transferType = getTransferType();
	mode = BODY;
	currRequest = "";
}

bool	Receiver::handleRequestBodyChunked(int fd)
{
	ssize_t	readByte;

	if (readSize > BUFFER_SIZE - 1)
		readByte = recv(fd, buffer, BUFFER_SIZE - 1, 0);
	else
		readByte = recv(fd, buffer, readSize, 0);
	if (readByte <= 0)
		throw std::runtime_error("close the client");
	buffer[readByte] = '\0';

	switch (chunkMode)
	{
	case CHUNKLEN:
		if (*buffer == '\r')
			chunkMode = CHUNKLENEND;
		else
			currRequest += std::tolower(*buffer);
		break;

	case CHUNKLENEND:
		if (*buffer != '\n' || currRequest[0] == '\r')
			throw std::runtime_error("close the client");
		readSize = 0;
		for (std::string::iterator it = currRequest.begin(); it <= currRequest.end(); ++it)
		{
			if (*it >= '0' && *it <= '9')
				readSize = readSize * 16 + *it - '0';
			else if (*it >= 'a' && *it <= 'f')
				readSize = readSize * 16 + *it - 'a' + 10;
			else
				break;
			if (readSize > MAXLENHEADER)
				throw std::runtime_error("close the client");
		}
		currRequest = "";
		if (readSize == 0)
		{
			currRequest = "\r\n";
			chunkMode = CHUNKLASTLINE;
			readSize = 1;
		}
		else
			chunkMode = CHUNKBODY;
		break;

	case CHUNKBODY:
		readSize -= readByte;
		currRequestRaw.append(buffer, readByte);
		if (readSize == 0)
		{
			chunkMode = CHUNKBODYEND;
			readSize = 2;
		}
		break;

	case CHUNKBODYEND:
		readSize -= readByte;
		currRequest.append(buffer, readByte);
		if (readSize == 0)
		{
			if (currRequest != "\r\n")
				throw std::runtime_error("close the client");
			chunkMode = CHUNKLEN;
			readSize = 1;
			currRequest = "";
		}
		break;
	
	case CHUNKLASTLINE:
		currRequest.append(buffer, readByte);
		if (currRequest.size() >= 4
			&& currRequest.compare(currRequest.size() - 4, 4, "\r\n\r\n") == 0)
			return true;
		break;
	}
	return false;
}

bool	Receiver::handleRequestBodyLength(int fd)
{
	ssize_t	readByte;

	if (readSize > BUFFER_SIZE - 1)
		readByte = recv(fd, buffer, BUFFER_SIZE - 1, 0);
	else
		readByte = recv(fd, buffer, readSize, 0);
	if (readByte <= 0)
		throw std::runtime_error("close the client");
	buffer[readByte] = '\0';
	readSize -= readByte;
	currRequestRaw.append(buffer, readByte);
	if (readSize == 0)
		return true;
	return false;
}

bool	Receiver::receiveRequest(int fd)
{
	if (mode == HEADERS)
	{
		handleRequestHeaders(fd);
		if (!(mode == BODY && transferType == UNDEFINED))
			return 0;
	}
	if ((mode == BODY && transferType == UNDEFINED)
		|| (mode == BODY && transferType == CHUNKED && handleRequestBodyChunked(fd))
		|| (mode == BODY && transferType == LENGTH && handleRequestBodyLength(fd)))
		return 1;
	return 0;
}
