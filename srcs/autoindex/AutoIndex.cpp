/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AutoIndex.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <cblonde@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 07:54:18 by cblonde           #+#    #+#             */
/*   Updated: 2025/03/27 22:25:19 by glaguyon         ###   ########.fr       */
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

	workDir = opendir(page._dirPath);
	if (!workDir)
		return "";
		//throw	std::runtime_error("unable to open dir " + std::string(dirPath));
	result = "<!DOCTYPE html>\n\t<html lang='en'>\n\t\t\
			  <head>\n\t\t\t\
			  <meta charset=\"UTF-8\"/>\n\t\t\t\
			  <meta name=\"viewport\" content=\"width=device-width, \
initial-scale=1.0\"/>\n\t\t\t\
			  <title>" + path + "</title>\n\t\t\
			  </head>\n\t\t\
			  <body>\n\t\t\t";
	while ((current = readdir(workDir)) != NULL)
		result += page.getLink(path, current->d_name);
	result += "\t\t</body>\n\t</html>";
	closedir(workDir);
	return (result);
}

std::string	AutoIndex::getLink(std::string path, std::string d_name)
{
	std::stringstream	str;
	std::string		protected_d_name;

	for (std::string::iterator it = d_name.begin(); it < d_name.end(); ++it)
	{
		switch (*it)
		{
			case '<':
				protected_d_name += "&lt";
				break;
			case '>':
				protected_d_name += "&gt";
				break;
			case '"':
				protected_d_name += "&quot";
				break;
			case '\'':
				protected_d_name += "&#39";
				break;
			case '&':
				protected_d_name += "&amp";
				break;
			default:
				protected_d_name += *it;
				break;
		}
	}
	str << "<a href='"
		<< "http://" << _host + ":" << _port
		<< path << (path.find_last_of("/") != path.size() - 1 ? "/" : "" )
		<< protected_d_name << "'>" << protected_d_name
		<< "</a><br/>\n\t\t\t";
	return (str.str());
}
