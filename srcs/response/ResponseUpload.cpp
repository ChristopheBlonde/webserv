/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseUpload.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/28 17:23:46 by glaguyon          #+#    #+#             */
/*   Updated: 2025/04/02 23:24:02 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include "Client.hpp"

bool	Response::handleFileUpload(int fd)
{	
	int			sent;
	int			chunkSize;
	std::map<int, FileData>::iterator it = _filesUpload.find(fd);

	if (it == _filesUpload.end())
		return (true);
	std::cerr << "handlefileupload\n";
	FileData	&file = it->second;
	chunkSize = file.size < FILE_BUFFER_SIZE ? file.size : FILE_BUFFER_SIZE;
	sent = write(fd, file.start + file.offset, chunkSize);
	if (sent <= 0)
		return (true);
	file.offset += sent;
	file.size -= sent;
	if (file.size == 0)
	{
		std::cout << GREEN << "File: " << file.fileName
			<< " upload successfully !" << RESET << std::endl;
		_filesUpload.erase(it);
		if (_filesUpload.empty())
			createResponseHeader();
		return (false);
	}
	return (true);
}

void	Response::openFiles(std::vector<FileData> vec)
{
	int	error = 0;

	for (std::vector<FileData>::iterator it = vec.begin(); it < vec.end(); ++it)
	{
		int	fd = -1;
		
		if (it->fileName.find_first_of("/") != std::string::npos)
			error = 400;
		else if (testAccess(uploadPath + it->fileName, EXIST))
			error = 409;
		if (error)
			break;
		fd = openFileUpload(uploadPath + it->filename);
		if (fd == -1)
		{
			error = 403;
			break;
		}
		_filesUpload[fd] = *it;
	}
	if (error)
	{
		for (it = _filesUpload.begin(); it != _filesUpload.end(); ++it)
		{
			close(it->first);
			std::remove(uploadPath + it->second.fileName);
		}
		_filesUpload.clear();
		throw error;
	}
}

std::vector<FileData>	Response::splitFiles(std::string boundary)
{
	size_t	pos = 0;
	
	while (pos < _body.size())
	{
		//chercher ligne qui commence par -- boundary
		//si ligne = -- boundary -- c'est la fin, return;
		//chercher fin des headers
		//extract le filename dans une map locale de headers
		//chercher le prochain \r\n-- boundary
	}
	throw 400;
}

void	Response::uploadFile(std::map<std::string, std::string> const &headers)
{
	this->uploadPath = _conf.getMount();
	if (_conf->getUploadDir()[0] != '/' && uploadPath.back() != '/')
		uploadPath += '/';
	uploadPath += _conf->getUploadDir() + _path.substr(_conf->getMount().size());
	if (!testAccess(uploadPath))
		return createError(404);

	std::string		boundary = headers["Content-Type"];
	size_t			boundaryPos = boundary.find("boundary=");
	std::vector<FileData>	fileVec;

	if (boundaryPos == std::string::npos)
		return createError(400);
	boundary = boundary.substr(boundaryPos + 9);
	boundary = boundary.substr(0, boundary.find(";"));
	try
	{
		fileVec = splitFiles(boundary);
		if (fileVec.empty())
			return createError(400);
		openFiles(fileVec);
	}
	catch (int code)
	{
		return createError(code);
	}

	std::map<int, FileData>::iterator it;

	for (it = _filesUpload.begin(); it != _filesUpload.end(); it++)
	{
		FileData tmp = it->second;
		std::cout << GREEN << "File Name: " << tmp.fileName
			<< std::endl << "File size: "
			<< tmp.size << RESET << std::endl;
		addFdToCluster(it->first, POLLOUT);
	}
}
