/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AutoIndex.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <cblonde@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 07:54:18 by cblonde           #+#    #+#             */
/*   Updated: 2025/04/02 18:03:41 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "AutoIndex.hpp"

AutoIndex::AutoIndex(const std::string &dirPath, const std::string &host)
	: _dirPath(dirPath),
	_host(host)
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
	}
	return (*this);
}
#include <iostream>

std::string AutoIndex::generate(const std::string &mount, const std::string &aliased,
	const std::string &dirPath, const std::string &host)
{
	std::cout << "autoindex\n";
	std::cout << "dirpath: " << dirPath << "\n";
	AutoIndex page(dirPath, host);//TODO 403 404 . ..
	std::string result;
	DIR	*workDir;
	struct dirent *current;
	std::string path(dirPath);

	workDir = opendir(page._dirPath.data());
	if (!workDir)
		return "";//throw ints or something
		//throw	std::runtime_error("unable to open dir " + std::string(dirPath));
	result = "<!DOCTYPE html>\n\t<html lang='en'>\n\t\t\
			  <head>\n\t\t\t\
			  <meta charset=\"UTF-8\"/>\n\t\t\t\
			  <meta name=\"viewport\" content=\"width=device-width, \
initial-scale=1.0\"/>\n\t\t\t\
			  <title>" + path + "</title>\n\t\t\
			  </head>\n\t\t\
			  <body style=\"background-color:#424549; text-color: #FFFFFF\">"
			  "<style>"
"        a {"
"            color: #FFFFFF; /* Discord's blurple color */"
"            text-decoration: none; /* Optional: Removes underline */"
"        }"

"        a:hover {"
"            color: #4752C4; /* Slightly darker blurple on hover */"
"        }"
"	ul {color: #FFFFFF;}"
"    </style>"
"<ul>"
			  "\n\t\t\t";
	while ((current = readdir(workDir)) != NULL)
		result += page.getLink(aliased + path.substr(mount.size()), current->d_name);
	result += "</ul>\t\t</body>\n\t</html>";
	closedir(workDir);
	return (result);
}

std::string	AutoIndex::getLink(std::string path, std::string d_name)
{
	std::string	htmlSafeName;
	std::string	attribSafeName;
	std::string	encodedPath;

	for (std::string::iterator it = d_name.begin(); it < d_name.end(); ++it)
	{
		switch (*it)
		{
			case '<':
				htmlSafeName += "&lt";
				attribSafeName += *it;
				break;
			case '>':
				htmlSafeName += "&gt";
				attribSafeName += *it;
				break;
			case '"':
				htmlSafeName += "&quot";
				attribSafeName += "&quot";
				break;
			case '\'':
				htmlSafeName += "&#39";
				attribSafeName += "&#39";
				break;
			case '&':
				htmlSafeName += "&amp";
				attribSafeName += "&amp";
				break;
			default:
				htmlSafeName += *it;
				attribSafeName += *it;
				break;
		}
	}
	if (d_name == ".")
		d_name = "";
	else if (d_name == "..")
	{
		d_name = "";
		path = path.substr(0, path.find_last_of("/", path.size() - 2) + 1);
	}
	encodedPath = urlEncode(path + d_name);
	return ("<li>"
		"<a href='http://" + _host + encodedPath
			+ "' download='" + attribSafeName + "'>[\u2B07]</a>"
		"<b>&#160&#160</b>"
		"<a href='http://" + _host + encodedPath + "'>" + htmlSafeName + "</a>"
		"</li>\n\t\t\t");
}
