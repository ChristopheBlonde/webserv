/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Client.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon           <skibidi@ohio.sus>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1833/02/30 06:67:85 by glaguyon          #+#    #+#             */
/*   Updated: 2025/03/20 22:07:00 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Client.hpp"

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
	currRequest = "";
	currRequestRaw = "";
	index = std::string::npos;
	transferType = UNDEFINED;
	expectedLen = -1;
}

bool	Client::checkEndRequest(ssize_t readByte)
{
//	if (index == std::string::npos)
//		index = currRequest.find("\r\n\r\n",
//			currRequest.size() - readbyte - 4);
//	if (index == std::string::npos)
//		return (false);
//	if (transferType == UNDEFINED)
//	{
//		size_t	transferHeader = currRequest.find("transfer-encoding:", 0, index);
//
//		if (transferHeader != std::string::npos)
//		{
//			transferHeader += std::strlen("transfer-encoding:"); 
//			while (currRequest[transferHeader] == " "
//				|| currRequest[transferHeader] == "\t")
//				++transferHeader;
//			if (currRequest.find("chunked", transferHeader) == transferHeader)
//				transferType = CHUNKED;
//		}
//		if (transferType == UNDEFINED)
//		{
//			transferHeader = currRequest.find("content-length:", 0, index);
//			if (transferHeader == std::string::npos)
//				return true;
//			expectedLen = std::strtol(
//				&currRequest[transferHeader + std::strlen("content-length:")],
//				NULL, 10);
//			if (expectedLen < 0)
//				expectedLen = 0;
//			transferType = LENGTH;
//		}
//	}
//	if (transferType == CHUNKED
//		&& currRequest.find("0\r\n\r\n", currRequest.size() - readByte - 5)//XXX tout 1 coup
//			!= std::string:npos)
//		return true;
//	if (transferType == LENGTH)
//	{
//		//?
//		end	= headers.find("\r\n", start);
//		if (end == std::string::npos)
//			return (false);
//		len = std::atoi(headers.substr(start, end - start).c_str());
//		if (str.size() >= body_start + len)
//			return (true);
//		else
//			return (false);
//	}
	return false;
}

//GET or DELETE: no header expected, if found discard body
//
void	Client::handleRequest()
{
//			ssize_t	readByte;
//			char	buffer[BUFFER_SIZE];
//
//			readByte = recv(fd, buffer, BUFFER_SIZE - 1, 0);
//			if (readByte <= 0)
//				clientCloseList.push_back(it->first);
//			else
//			{
//				buffer[readByte] = '\0';
//			}
//	std::string	s_low = s;
//
//	currRequestRaw += s;
//	for (std::string::iterator it = s_low.begin(); it < s_low.end(); ++it)
//		*it = std::tolower(*it);
//	currRequest += s_low;
//	if (checkEndRequest())
//	{
//		//lancer la Request
//		std::cout << CYAN << requests[fd.fd] << std::endl << RESET;
//		Requests req(requests[fd.fd]);
//		Response *res = new Response(req);
//		res->setSocket(fd.fd);
//		std::cout << GREEN << "new Request create with Socket: "
//			<< res->getSocket() << RESET << std::endl;
//		ress[fd.fd] = res;
//		requests.erase(fd.fd);
//
//		resetRequest();
//	}
}

void	Client::handleResponse(int fd)
{
}
