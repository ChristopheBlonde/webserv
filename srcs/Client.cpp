/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon           <skibidi@ohio.sus>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1833/02/30 06:67:85 by glaguyon          #+#    #+#             */
/*   Updated: 2025/03/28 13:43:49 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"
#include "Cluster.hpp"

Client::Client() :
	on(false),
	c(NULL),
	fd(-1),
	ip(-1),
	port(-1),
	hostName("")
{
	resetRequest();
}

Client::Client(int fd, struct sockaddr_in addr, Cluster *c) :
	on(false),
	c(c),
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
		if (res->ai_canonname)
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

int	Client::getTransferType()
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
		c->closeClient(fd);
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
		c->closeClient(fd);
		readSize = 1;
		return LENGTH;
	}
	return LENGTH;
}

void	Client::handleRequestHeaders()
{
	ssize_t	readByte;

	readByte = recv(fd, buffer, 1, 0);
	if (readByte <= 0)
	{
		c->closeClient(fd);
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
	transferType = getTransferType();
	mode = BODY;
}

bool	Client::handleRequestBodyChunked()
{
	ssize_t	readByte;

	if (readSize > BUFFER_SIZE - 1)
		readByte = recv(fd, buffer, BUFFER_SIZE - 1, 0);
	else
		readByte = recv(fd, buffer, readSize, 0);
	if (readByte <= 0)
	{
		c->closeClient(fd);
		return false;
	}
	else
		buffer[readByte] = '\0';

	switch (chunkMode)
	{
	case CHUNKLENEND:
		if (*buffer != '\n')
		{
			c->closeClient(fd);
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
				c->closeClient(fd);
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
				c->closeClient(fd);
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

bool	Client::handleRequestBodyLength()
{
	ssize_t	readByte;

	if (readSize > BUFFER_SIZE - 1)
		readByte = recv(fd, buffer, BUFFER_SIZE - 1, 0);
	else
		readByte = recv(fd, buffer, readSize, 0);
	if (readByte <= 0)
	{
		c->closeClient(fd);
		return false;
	}
	buffer[readByte] = '\0';
	readSize -= readByte;
	currRequestRaw += buffer;
	if (readSize == 0)
		return true;
	return false;
}

void	Client::handleRequest()
{
	if (mode == HEADERS)
	{
		handleRequestHeaders();
		if (!(mode == BODY && transferType == UNDEFINED))
			return;
	}
	if ((mode == BODY && transferType == UNDEFINED)
		|| (mode == BODY && transferType == CHUNKED && handleRequestBodyChunked())
		|| (mode == BODY && transferType == LENGTH && handleRequestBodyLength()))
	{
		//TODO remove pointer maybe
//		std::cout << CYAN << requests[fd.fd] << std::endl << RESET;
		Requests req(currRequestRaw, *this);
		req.setConf(c->getRoute(c->getServer(fd, req.getHost()), req.getPath()));
		Response *res = new Response(req, *this,
				c->getServer(fd, req.getHost()));
		res->setSocket(fd);
		responses.push(res);
//		std::cout << GREEN << "new Request create with Socket: "
//			<< res->getSocket() << RESET << std::endl;
		//XXX
		resetRequest();
	}
}

void	Client::handleResponse()
{
	if (responses.empty())
		return;
	
	PollFd	pfd(fd);

	pfd.revents = c->getRevents(fd);
	//POLLOUT is on
	if (!responses.front()->handleInOut(pfd))
	{
		delete responses.front();
		responses.pop();
		removeFds();
		for (std::vector<int>::iterator it = responseFds.begin();
			it < responseFds.end(); ++it)
			responseFdRemoveList.push_back(*it);
		removeFds();
		return ;
	}
	removeFds();
	addFds();
	for (std::vector<int>::iterator it = responseFds.begin(); it < responseFds.end(); ++it)
	{
		PollFd		pfd(*it);

		pfd.revents = c->getRevents(*it);
		if (!responses.front()->handleInOut(pfd))
			responseFdRemoveList.push_back(*it);

	}
	removeFds();
	addFds();
}

void	Client::removeFds()
{
	for (std::vector<int>::iterator it = responseFdRemoveList.begin();
		it < responseFdRemoveList.end(); ++it)
	{
		std::cerr << *it << " close\n";
		close(*it);
		responseFds.erase(std::find(responseFds.begin(), responseFds.end(), *it));
		c->removeFd(*it);
	}
	responseFdRemoveList.clear();
}

Client::~Client()
{
	if (!on)
		return;
	close(fd);
	while (!responses.empty())
	{
		delete responses.front();
		responses.pop();
	}
	removeFds();
	for (std::vector<int>::iterator it = responseFds.begin(); it < responseFds.end(); ++it)
		responseFdRemoveList.push_back(*it);
	removeFds();
}

void	Client::addResponseFd(PollFd pfd)
{
	responseFdAddList.push_back(pfd);
}

void	Client::addFds()
{
	for (std::vector<PollFd>::iterator it = responseFdAddList.begin();
		it < responseFdAddList.end(); ++it)
	{
		if (std::find(responseFds.begin(),
					responseFds.end(), it->fd) == responseFds.end())
		{
			std::cerr << it->fd << " add\n";
			c->addFd(*it);
			responseFds.push_back(it->fd);
		}
	}
	responseFdAddList.clear();
}
