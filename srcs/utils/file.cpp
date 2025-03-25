/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   file.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 11:47:28 by cblonde           #+#    #+#             */
/*   Updated: 2025/03/25 15:02:15 by cblonde          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <utils.hpp>

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

int	openDir(std::string path, std::string &file, std::vector<std::string> &files)
{
	DIR		*dir;
	struct	dirent *dirp;
	int		fd = -1;
	std::vector<std::string>::iterator it;

	dir = opendir(path.c_str());
	if (!dir)
	{
		std::cerr << RED << "opendir: " << path << " " << strerror(errno)
			<< RESET << std::endl;
		return (-2);
	}
	while ((dirp = readdir(dir)) != NULL)
	{
		if (file.empty())
		{
			for (it = files.begin(); it != files.end(); it++)
			{
				if (!std::string(dirp->d_name).compare(*it))
				{
					file = *it;
					fd = getFile(path + "/" + *it);
					closedir(dir);
					return (fd);
				}
			}
		}
		else if (!file.empty() && !std::string(dirp->d_name).compare(file))
		{
			fd = getFile(path + "/" + file);
			closedir(dir);
			return (fd);
		}
	}
	closedir(dir);
	return (fd);
}

std::string	getFileType(std::string file)
{
	size_t		index;

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
	headers["Connection"] = "Connection: ";
	headers["Date"] = "Date: ";
	headers["Last-Modified"] = "Last-Modified: ";
}

bool	testAccess(std::string path, int test)
{
	int	res;
	DIR	*dir;

	switch (test)
	{
		case 0:
			res = access(path.data(), F_OK);
			if (res)
				return (false);
			break ;
		case 1:
			res = access(path.data(), R_OK);
			if (res)
				return (false);
			break ;
		case 2:
			res = access(path.data(), W_OK);
			if (res)
				return (false);
			break ;
		case 3:
			res = access(path.data(), X_OK);
			if (res)
				return (false);
			break ;
		case 4:
			dir = opendir(path.data());
			if (!dir)
				return (false);
			closedir(dir);
			break ;
		default:
			break ;
	}
	return (true);
}
