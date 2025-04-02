/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AutoIndex.cpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <cblonde@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 07:54:18 by cblonde           #+#    #+#             */
/*   Updated: 2025/04/02 19:29:48 by glaguyon         ###   ########.fr       */
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

std::string AutoIndex::htmlEncode(std::string s, bool isAttribute)
{
	std::string	htmlSafeName;
	std::string	attribSafeName;

	for (std::string::iterator it = s.begin(); it < s.end(); ++it)
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
	if (isAttribute)
		return attribSafeName;
	return htmlSafeName;
}

std::string AutoIndex::generate(const std::string &mount, const std::string &aliased,
	const std::string &dirPath, const std::string &host)
{
	AutoIndex page(dirPath, host);
	std::string result;
	DIR	*workDir;
	struct dirent *current;
	std::string path(dirPath);
	
	if (!testAccess(path, DIRACCESS))
		throw 404;
	workDir = opendir(page._dirPath.data());
	if (!workDir)
		throw 403;
	result = "<!DOCTYPE html>\n"
			"\t<html lang='en'>\n"
			"\t\t<head>\n"
			"\t\t\t<meta charset=\"UTF-8\"/>\n"
			"\t\t\t<meta name=\"viewport\" content=\"width=device-width,"
								"initial-scale=1.0\"/>\n"
			"\t\t\t<title> index " + htmlEncode(aliased + path.substr(mount.size()), 0)
								+ "</title>\n"
			"\t\t</head>\n"
			"\t\t<body>"
			"\t\t\t<style>"
			"\t\t\t\tbody {background-color:#424549;}\n"
			"\t\t\t\ta {color: #FFFFFF;text-decoration: none;}\n"
			"\t\t\t\ta:hover {color: #4866cf;}\n"
			"\t\t\t\t.dl {color: #7289da;}\n"
			"\t\t\t\t.dl:hover {color: #4866cf;}\n"
			"\t\t\t\tul {color: #FFFFFF;}\n"
			"\t\t\t</style>\n"
			"\t\t\t<ul>\n";
	while ((current = readdir(workDir)) != NULL)
		result += "\t\t\t\t"
			+ page.getLink(aliased + path.substr(mount.size()), current->d_name)
			+ "\n";
	result += "\t\t\t</ul>\n"
			"\t\t</body>\n"
			"\t</html>";
	closedir(workDir);
	return (result);
}

std::string	AutoIndex::getLink(std::string path, std::string d_name)
{
	std::string	htmlSafeName = htmlEncode(d_name, 0);
	std::string	attribSafeName = htmlEncode(d_name, 1);
	std::string	encodedPath;
	
	if (d_name == ".")
		d_name = "";
	else if (d_name == "..")
	{
		d_name = "";
		path = path.substr(0, path.find_last_of("/", path.size() - 2) + 1);
	}
	encodedPath = urlEncode(path + d_name);
	return ("<li>"
		"<a class=\"dl\" href='http://" + _host + encodedPath
			+ "' download='" + attribSafeName + "'>[\u2B07]</a>"
		"<b>&#160&#160</b>"
		"<a href='http://" + _host + encodedPath + "'>" + htmlSafeName + "</a>"
		"</li>");
}
