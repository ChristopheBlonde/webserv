/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   file.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 11:47:28 by cblonde           #+#    #+#             */
/*   Updated: 2025/04/07 22:21:58 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"

int	getFile(std::string path)
{
	int	fd;

	fd = open(path.c_str(), O_RDONLY | O_NONBLOCK);
	if (fd < 0)
	{
		std::cerr << RED << "Error: getFile: " << std::strerror(errno) << RESET
			<< std::endl;
		return (-1);
	}
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
    mime_types["php"] = "application/x-httpd-php";
    mime_types["sh"] = "application/x-sh";
    mime_types["py"] = "application/x-python-code";
    mime_types["bla"] = "application/42-cgi";
}

void initResponseHeaders(std::map<std::string, std::string> &headers)
{
	headers["Content-Type"] = "Content-Type: ";
	headers["Content-Length"] = "Content-Length: ";
	headers["Connection"] = "Connection: ";
	headers["Date"] = "Date: ";
	headers["Last-Modified"] = "Last-Modified: ";
}

bool	testAccess(std::string path, t_access test)
{
	int	res;

	res = 0;
	switch (test)
	{
		case EXIST:
			res = access(path.data(), F_OK);
			break ;
		case READABLE:
			res = access(path.data(), R_OK);
			break ;
		case WRITEABLE:
			res = access(path.data(), W_OK);
			break ;
		case EXECUTABLE:
			res = access(path.data(), X_OK);
			break ;
		case DIRACCESS:
			struct stat	statbuf;
			res = !(stat(path.data(), &statbuf) == 0
					&& S_ISDIR(statbuf.st_mode));
			break ;
		default:
			break ;
	}
	if (res)
	{
		std::cerr << RED << "Error: Access: " << strerror(errno)
			<< RESET << std::endl;
		return (false);
	}
	return (true);
}

int	openFileUpload(std::string path)
{
	int	fd = -1;

	fd = open(path.data(), O_WRONLY | O_CREAT | O_TRUNC | O_NONBLOCK, 0644);
	if (fd < 0)
		std::cerr << RED << "Error: Open fd upload: "
			<< strerror(errno) << RESET << std::endl;
	return (fd);
}
