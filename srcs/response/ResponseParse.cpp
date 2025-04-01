/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseParse.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <cblonde@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/29 08:25:35 by cblonde           #+#    #+#             */
/*   Updated: 2025/04/01 11:12:48 by cblonde          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Response.hpp>

void	Response::checkConnection(std::map<std::string,
		std::string> const &headers)
{
	std::map<std::string, std::string>::const_iterator it;

	if (_autoIndex || !_conf->getRedirection().empty())
	{
		_headers["Connection"] += "close";
		if (!_conf->getRedirection().empty())
			_status = 302;
	}
	else
	{
		it = headers.find("Connection");
		if (it != headers.end())
			_headers["Connection"] += it->second;
	}
}


bool	Response::checkMethod(std::string method)
{
	std::cout << RED << "METHOD: " << method << RESET << std::endl;
	std::set<std::string> &methods(_conf->getAcceptedMethods());
	std::set<std::string>::iterator it;
	for (it = methods.begin(); it != methods.end(); it++)
		std::cout << GREEN << "Method in conf: " << *it << RESET <<std::endl;
	it = methods.find(method);
	if (it == methods.end())
	{
		std::cout << YELLOW << "Error: Methods: " << RESET << std::endl;
		createError(405);
		return (false);
	}
	return (true);
}

bool	Response::checkContentLen(std::map<std::string,
		std::string> const &headers)
{
	std::map<std::string, std::string>::const_iterator it;
	long	contentLen;
	size_t	maxSize = _server.getMaxSize();

	if (maxSize == 0)
		return (true);
	it = headers.find("Content-Length");
	if (it != headers.end())
	{
		contentLen = strtol(it->second.data(), NULL, 10);
		if (maxSize < static_cast<size_t>(contentLen))
		{
			createError(413);
			return (false);
		}
	}
	return (true);
}

void	Response::getStatFile(std::string path)
{
	struct stat res;
	int			status;
	time_t		ttime;
	struct tm	*tmTime;
	char		buffer[1024];
	size_t		size;

	ttime = time(NULL);
	tmTime = gmtime(&ttime);
	strftime(buffer, 1024, "%a, %d %b %Y %T GMT", tmTime);
	std::cout << CYAN << "Stat: Date: " << buffer << RESET << std::endl;
	_headers["Date"] += buffer;
	if (path.empty())
	{
		_headers.erase("Last-Modified");
		return ;
	}
	status = stat(path.c_str(), &res);
	if (status)
	{
		std::cerr << RED << "Error: stat: " << strerror(errno)
			<< RESET << std::endl;
		return ;
	}
	#ifdef __linux__
		ttime = res.st_mtim.tv_sec;
	#else
		ttime = res.st_mtimespec.tv_sec;
	#endif
	tmTime = gmtime(&ttime);
	strftime(buffer, 1024, "%a, %d %b %Y %T GMT", tmTime);
	std::cout << CYAN << "Stat: Last-Modified: " << buffer << RESET << std::endl;
	_headers["Last-Modified"] += buffer;
	size = res.st_size;
	_buffer.reserve(size * sizeof(unsigned char));
	std::cout << CYAN << "Stat: Size: " << size << RESET << std::endl;
}

std::string	getResponseTypeStr(int stat)
{
	std::map<int,std::string> names;

	names[200] = "OK";
	names[301] = "Moved Permanently";
	names[302] = "Found";
	names[400] = "Bad Request";
	names[403] = "Forbidden";
	names[404] = "Not Found";
	names[405] = "Method Not Allowed";
	names[410] = "Gone";
	names[413] = "Content Too Large";
	names[500] = "Internal Server Error";

	return (names[stat]);
}

std::string	getContentError(int stat)
{
	std::map<int,std::string> content;

	content[200] = "OK";
	content[400] = "the server would not process the request due to something \
the server considered to be a client error";
	content[403] = "Insufficient permissions to a resource or action";
	content[404] = "Page not found";
	content[405] = "Target resource doesn't support this method";
	content[410] = "Target resource is no longer available";
	content[413] = "Request entity was larger than limits defined by server";
	content[500] = "Server encountered an unexpected condition that prevented \
it from fulfilling the request";

	return (content[stat]);
}
