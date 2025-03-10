/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AutoIndex.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <cblonde@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 07:54:18 by cblonde           #+#    #+#             */
/*   Updated: 2025/03/10 17:24:48 by cblonde          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <AutoIndex.hpp>

AutoIndex::AutoIndex(void)
{
	return ;
}

AutoIndex::AutoIndex(char const *dirPath, std::string const &host, int port)
	: _dirPath(dirPath),
	_host(host),
	_port(port)
{
	return ;
}

AutoIndex::AutoIndex(AutoIndex const &src)
{
	*this = src;
	return ;
}

AutoIndex::~AutoIndex(void)
{
	return ;
}

AutoIndex	&AutoIndex::operator=(AutoIndex const &rhs)
{
	if (this != &rhs)
	{
		_dirPath = rhs._dirPath;
		_host = rhs._host;
		_port = rhs._port;
	}
	return (*this);
}

std::string	AutoIndex::generate(char const *dirPath, std::string const &host,
		int port)
{
	AutoIndex page(dirPath, host, port);
	std::string result;
	DIR	*workDir;
	struct dirent *current;
	std::string path(dirPath);

	std::cout << CYAN << "Debug: AutoIndex: dirPath:" << std::string(dirPath)
		<< " host: " << host << " port " << port << RESET << std::endl;

	workDir = opendir(page._dirPath);
	if (!workDir)
	{
		std::cerr << RED << "Error: openDir: " << dirPath << " "
			<< strerror(errno) << RESET << std::endl;
		return (result);
	}
	result = "<!DOCTYPE html>\n\t<html lang='en'>\n\t\t\
			  <head>\n\t\t\t\
			  <meta charset=\"UFT-8\"/>\n\t\t\t\
			  <meta name=\"viewport\" content=\"width=device-width, \
initial-scale=1.0\"/>\n\t\t\t\
			  <title>" + path + "</title>\n\t\t\
			  </head>\n\t\t\
			  <body>\n\t\t\t";
	while ((current = readdir(workDir)) != NULL)
	{
		result += page.getLink(path, current->d_name);
	}
	result += "\t\t</body>\n\t</html>";
	return (result);
}

std::string	AutoIndex::getLink(std::string path, std::string d_name)
{
	std::stringstream	str;
	std::cout << GREEN << "Link: path:" << path << " d_name:" << d_name << RESET
		<< std::endl;

	str << "<a href='" << "http://" << _host + ":" << _port
		<< path << (path.find_last_of("/") != path.size() - 1 ? "/" : "" )
		<< d_name << "'>" << d_name << "</a><br/>\t\t\t";
	return (str.str());
}
