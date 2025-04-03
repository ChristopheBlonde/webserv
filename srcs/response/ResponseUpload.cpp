/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseUpload.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/28 17:23:46 by glaguyon          #+#    #+#             */
/*   Updated: 2025/04/03 05:21:06 by glaguyon         ###   ########.fr       */
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
		
		std::cout << "opening: " << it->fileName << "\n";
		if (testAccess(uploadPath + it->fileName, EXIST))
		{
			error = 409;
			break;
		}
		fd = openFileUpload(uploadPath + it->fileName);
		if (fd == -1)
		{
			error = 403;
			break;
		}
		_filesUpload[fd] = *it;
	}
	if (error)
	{
		for (std::map<int, FileData>::iterator it = _filesUpload.begin();
			it != _filesUpload.end(); ++it)
		{
			close(it->first);
			std::remove((uploadPath + it->second.fileName).c_str());
		}
		_filesUpload.clear();
		throw error;
	}
}

static int	parseHeader(const std::string &body, size_t pos,
		std::map<std::string, std::string> &headers)
{
	size_t					end = body.find("\r\n", pos);
	std::string				line;
	std::pair<std::string, std::string>	pair;

	if (end == std::string::npos)
		return 1;
	line = body.substr(pos, end - pos);
	end = line.find(":");
	if (end == std::string::npos)
		return 1;
	pair.first = line.substr(0, end);
	formatHeader(pair.first);
	pair.second = line.substr(end + 1);
	trim(pair.second);
	headers[pair.first] = pair.second;
	return 0;
}

static std::string	getFilename(std::map<std::string, std::string> &headers)
{
	std::string	fname = "";
	std::string	s = headers["Content-Disposition"] + ";";
	size_t		delim;
	size_t		oldDelim;

	if (s == "")
		return "";
	std::cout << s << " awow\n";
	if (s.find("form-data") != 0)
		return "";
	delim = s.find_first_not_of(" \t", 9);
	if (delim == std::string::npos || s[delim] != ';')
		return "";
	std::cout << delim << " awow\n";
	oldDelim = delim;
	delim = s.find(";", delim + 1);
	while (delim != std::string::npos)
	{
		std::string	word = s.substr(oldDelim + 1, delim - oldDelim - 1);
		trim(word);
		
		std::cout << word << "\n";
		if (word.compare(0, 9, "filename=") == 0)
		{
			if (fname != "" || word[9] != '"')
				return "";
			
			size_t	end = word.find("\"", 11);
			
			std::cout << end << " end\n"; 
			if (end == std::string::npos
				|| (word.find_first_not_of(" \t", end + 1) != std::string::npos
					&& word[word.find_first_not_of(" \t", end + 1)] != ';'))
				return "";
			fname = word.substr(10, end - 10);
			if (fname.find("/") != std::string::npos)
				return "";
		}
		oldDelim = delim;
		delim = s.find(";", delim + 1);
	}
	std::cout << "returned: " << fname << "\n";
	return fname;
}

/*
--BOUNDARY\r\n
[Part Headers]\r\n
\r\n
[Part Data]\r\n
--BOUNDARY\r\n
*/
//until --BOUNDARY--\r\n
std::vector<FileData>	Response::splitFiles(std::string boundary)
{
	std::vector<FileData>	vec;
	size_t			pos = 2;
	
	if (_body.compare(0, 2, "--"))
		throw 400;
	while (pos < _body.size())
	{
		std::cout << "SALUT BATARD\n";
		std::cout << "|||" << _body.data() + pos << "|||\n";
		if (_body.compare(pos, boundary.size(), boundary)
			|| pos + boundary.size() >= _body.size())
			throw 400;
		pos += boundary.size();
		std::cout << "SALUT BATARD2\n";
		std::cout << "|||" << _body.data() + pos << "|||\n";
		if (_body.compare(pos, 4, "--\r\n") == 0)
			return vec;
		if (_body.compare(pos, 2, "\r\n")
			|| pos + 2 > _body.size())
		{
			std::cout << "bah oui\n";
			throw 400;
		}
		pos += 2;
		std::cout << "SALUT BATARD3\n";
		std::cout << "|||" << _body.data() + pos << "|||\n";
		
		std::map<std::string, std::string>	headers;
		while (1)
		{
			if (parseHeader(_body, pos, headers)
				|| _body.find("\r\n", pos) + 2 >= _body.size())
				throw 400;
			pos = _body.find("\r\n", pos) + 2;
		std::cout << "SALUT WOW\n";
		std::cout << "|||" << _body.data() + pos << "|||\n";
			if (_body.compare(pos, 2, "\r\n") == 0)
				break;
		}
		pos += 2;
		std::cout << "SALUT BATARD4\n";
		std::cout << "|||" << _body.data() + pos << "|||\n";

		FileData	fdata = {.fileName = getFilename(headers),
						.start = _body.data() + pos,
						.size = 0,
						.offset = 0};
		if (fdata.fileName == "")
			throw (400);
		std::cout << "filename found: " << fdata.fileName << "\n";
		size_t end = _body.find("\r\n--" + boundary, pos);
		if (end == std::string::npos)
			throw 400;
		fdata.size = end - pos;
		pos = end + 4;
		std::cout << "SALUT BATARD5\n";
		std::cout << "|||" << _body.data() + pos << "|||\n";
		std::cout << "|||" << boundary << "|||\n";
		vec.push_back(fdata);
		std::cout << "added file\n";
	}
	throw 400;
}

void	Response::uploadFile(std::map<std::string, std::string> const &headers)
{
	this->uploadPath = _conf->getMount();
	if (_conf->getUploadDir()[0] != '/' && uploadPath[uploadPath.size() - 1] != '/')
		uploadPath += '/';
	uploadPath += _conf->getUploadDir() + "/";
	std::cout << uploadPath << "\n";
	if (!testAccess(uploadPath, DIRACCESS))
		return createError(404);

	std::string		boundary = headers.at("Content-Type");
	size_t			boundaryPos = boundary.find("boundary=");
	std::vector<FileData>	fileVec;

	if (boundaryPos == std::string::npos)
		return createError(400);
	boundary = boundary.substr(boundaryPos + 9);
	boundary = boundary.substr(0, boundary.find(";"));
	try
	{
		fileVec = splitFiles(boundary);
		std::cout << "splitfiles done\n";
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
