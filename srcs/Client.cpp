/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon           <skibidi@ohio.sus>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1833/02/30 06:67:85 by glaguyon          #+#    #+#             */
/*   Updated: 2025/03/25 20:29:03 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Cluster.hpp"

Client::Client() :
	on(false),
	fd(-1),
	ip(-1),
	port(-1),
	hostName("")
{
	resetRequest();
}

Client::Client(int fd, struct sockaddr_in addr) :
	on(false),
	fd(fd),
	ip(addr.sin_addr.s_addr),
	port(addr.sin_port),
	hostName("")
{
	char                    ipStrChar[INET_ADDRSTRLEN];
        std::stringstream       ss;
	struct addrinfo		hints;
	struct addrinfo		*res = 0;

        inet_ntop(AF_INET, &addr.sin_addr, ipStrChar, sizeof(ipStrChar));
        ipStr = ipStrChar;
        ss << ntohs(port);
        portStr = ss.str();
	memset(&hints, 0, sizeof(hints));
	hints.ai_flags = AI_CANONNAME;
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_protocol = IPPROTO_TCP;
	if (getaddrinfo(ipStrChar, portStr.c_str(), &hints, &res) == 0)
	{
		hostName = res->ai_canonname;
		freeaddrinfo(res);
	}
	else
		hostName = "";

	int		opt = 1;
	socklen_t	optLen = sizeof(opt);

	getsockopt(fd, SOL_SOCKET, SO_RCVBUF, &opt, &optLen);
	bufferSize = opt;
	resetRequest();
}

Client::~Client()
{
	if (!on)
		return;
	close(fd);
}

uint64_t	Client::getIp()
{
	return ip;
}

std::string	Client::getIpStr()
{
	return ipStr;
}

uint16_t	Client::getPort()
{
	return port;
}

std::string	Client::getPortStr()
{
	return portStr;
}

std::string	Client::getHostName()
{
	return hostName;
}

size_t	Client::getBufferSize()
{
	return bufferSize;
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
	if (currRequest.find("content-length:", transferHeader + 1) != std::string::npos)
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
	{
		handleRequestHeaders(c);
		if (!(mode == BODY && transferType == UNDEFINED))
			return;
	}
	if ((mode == BODY && transferType == UNDEFINED)
		|| (mode == BODY && transferType == CHUNKED && handleRequestBodyChunked(c))
		|| (mode == BODY && transferType == LENGTH && handleRequestBodyLength(c)))
	{
		//XXX
//		std::cout << CYAN << requests[fd.fd] << std::endl << RESET;
		Requests req(currRequestRaw);
		Response *res = new Response(req);
		res->setSocket(fd);
//		std::cout << GREEN << "new Request create with Socket: "
//			<< res->getSocket() << RESET << std::endl;
		ress[fd] = res;
		//XXX
		resetRequest();
	}
}

void	Client::handleResponse(Cluster &c)
{
	//XXX
	for(size_t i = 0; i < c.fds.size(); i++)
	{
		//std::cout << it->first << "\n";
		PollFd pfd = c.getPollFd(c.fds[i].fd);

		if (pfd.revents & POLLERR)
		{//?
		}
		//XXX il faut check poll in ici de preference
		
		std::map<int, Response *>::iterator  res;
		std::map<int, Response *>::iterator  file;
		res = ress.find(pfd.fd);
		file = files.find(pfd.fd);
		if (res != ress.end())
		{
			if (!res->second->handleInOut(pfd))
			{
				std::cout << "dans delete" << std::endl;
				delete res->second;
				ress.erase(res);
				//_c.fds.erase(_c.fds.begin() + i--);
			}
		}
		else if (file != files.end())
		{
			if (!file->second->handleInOut(pfd))
			{
				close (file->first);
				files.erase(file);
			c.fds.erase(c.fds.begin() + i--);
			}
		}
		handleFiles(c);

	}
	//XXX
}

void	Client::handleFiles(Cluster &c)//XXX (or not idk)
{
	PollFd	res;

	for (std::map<int, Response *>::iterator it = ress.begin();
			it != ress.end(); it++)
	{
		res.fd = it->second->getFileFd();
		if (res.fd < 0 || (files.find(res.fd) != files.end()))
			continue ;
		res.events = POLLIN;
		res.revents = 0;
		c.addFd(res);
		files.insert(std::make_pair(res.fd, it->second));
	}
}
