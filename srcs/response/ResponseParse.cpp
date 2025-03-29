/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseParse.cpp                                  :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <cblonde@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/29 08:25:35 by cblonde           #+#    #+#             */
/*   Updated: 2025/03/29 08:35:07 by cblonde          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Response.hpp>

void	Response::checkConnection(std::map<std::string,
		std::string> const &headers)
{
	std::map<std::string, std::string>::const_iterator it;

	if (_autoIndex)
		_headers["Connection"] += "close";
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

void	Response::isReferer(std::map<std::string, std::string> const &headers)
{
	std::map<std::string, std::string>::const_iterator it;

	it = headers.find("Referer");
	std::string referer = it != headers.end() ? it->second : "";
	if (!referer.empty())
	{
		size_t index = referer.find(to_string(_port));
		referer = referer.substr(index + to_string(_port).size());
		handleBadPath(referer);
	}
	std::string routePath = _conf->getName();
	std::cout << RED << "pathRoute: " << routePath << RESET <<std::endl;
	if (routePath == _path || routePath == referer)
		routePath = _conf->getRoot();
	else
		routePath = _path;

	if (*(routePath.begin() + routePath.size() - 1) == '/')
		routePath = routePath.substr(0, routePath.size() - 1);
	if (!referer.empty() && _path != routePath)
		_path = routePath + _path;
	else
		_path = routePath;
}

void	Response::getStatFile(std::string path)
{
	struct stat res;
	int			status;
	time_t		ttime;
	struct tm	*tmTime;
	char		buffer[1024];
	size_t		size;

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
	ttime = time(NULL);
	tmTime = gmtime(&ttime);
	strftime(buffer, 1024, "%a, %d %b %Y %T GMT", tmTime);
	std::cout << CYAN << "Stat: Date: " << buffer << RESET << std::endl;
	_headers["Date"] += buffer;
	size = res.st_size;
	_buffer.reserve(size * sizeof(unsigned char));
	std::cout << CYAN << "Stat: Size: " << size << RESET << std::endl;
}

std::string	getResponseTypeStr(int stat)
{
	std::map<int,std::string> names;

	names[200] = "OK";
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
