/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   file.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 11:47:28 by cblonde           #+#    #+#             */
/*   Updated: 2025/03/13 13:57:49 by cblonde          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <webserv.hpp>

int	getFile(std::string path)
{
	int	fd;

	fd = open(path.c_str(), O_RDONLY);
	if (fd < 0)
	{
		std::cerr << RED << "Error: getFile: " << std::strerror(errno) << RESET
			<< std::endl;
		return (-1);
	}
	return (fd);
}

std::string	getFileName(std::string &path)
{
	std::string				name;
	std::string				extention;
	size_t					pos;
	std::string::iterator	it;

	if (*(it = path.end() - 1) == '/')
		path.erase(it);
	pos = path.find_last_of(".");
	if (pos == std::string::npos)
		return (path);
	extention = path.substr(pos + 1);
	pos = path.find_last_of("/");
	name = path.substr(pos + 1);
	path = path.substr(0, pos);
	return (name);
}

int	openDir(std::string path, std::string file)
{
	DIR		*dir;
	struct	dirent *dirp;
	int		fd = -1;

	path = "." + path;
	dir = opendir(path.c_str());
	if (!dir)
	{
		std::cerr << RED << "opendir: " << path << " " << strerror(errno)
			<< RESET << std::endl;
		return (fd);
	}
	while ((dirp = readdir(dir)) != NULL)
	{
		/* find conf file.mime */
		if (file.empty()
				&& !std::string(dirp->d_name).compare("index.html"))
		{
			fd = getFile(path + "/index.html");
			closedir(dir);
			return (fd);
		}
		else if (!file.empty() && !std::string(dirp->d_name).compare(file))
		{
			fd = getFile(path + "/" + file);
			closedir(dir);
			return (fd);
		}
//	std::cout << CYAN << dirp->d_name << ": file: "
//		<< file << " dir: " << path
//		<< (!std::string(dirp->d_name).compare("index.html")
//				? " true" : " false") << RESET << std::endl;
	}
	closedir(dir);
	return (fd);
}

std::string	getFileType(std::string path)
{
	std::string	file;
	size_t		index;

	file = getFileName(path);
	/* conf index */
	if (!file.compare(path))
		return ("html");
	index = file.find_last_of(".");
	if (index == std::string::npos)
		return ("error");
	return (file.substr(index + 1));
}

void	initMimeTypes(std::map<std::string, std::string> &mime_types)
{
    mime_types["html"] = "text/html";
    mime_types["htm"] = "text/html";
    mime_types["css"] = "text/css";
    mime_types["js"] = "text/javascript";
    mime_types["txt"] = "text/plain";
    mime_types["xml"] = "text/xml";
    mime_types["gif"] = "image/gif";
    mime_types["jpg"] = "image/jpeg";
    mime_types["jpeg"] = "image/jpeg";
    mime_types["png"] = "image/png";
    mime_types["svg"] = "image/svg+xml";
    mime_types["ico"] = "image/x-icon";
    mime_types["json"] = "application/json";
    mime_types["pdf"] = "application/pdf";
    mime_types["zip"] = "application/zip";
    mime_types["gz"] = "application/gzip";
    mime_types["tar"] = "application/x-tar";
    mime_types["bz2"] = "application/x-bzip2";
}

void initResponseHeaders(std::map<std::string, std::string> &headers)
{
	headers["Content-Type"] = "Content-Type: ";
	headers["Content-Length"] = "Content-Length: ";
	headers["Set-Cookie"] = "Set-Cookie: ";
}
