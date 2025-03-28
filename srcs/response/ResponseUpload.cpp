/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseUpload.cpp                                 :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/28 17:23:46 by glaguyon          #+#    #+#             */
/*   Updated: 2025/03/28 17:25:06 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Response.hpp>
#include <Client.hpp>

bool	Response::handleFileUpload(int fd)
{	
	int			sent;
	int			chunkSize;
	std::map<int, FileData>::iterator it = _filesUpload.find(fd);

	if (it == _filesUpload.end())
		return (true);
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

std::string	Response::handleBoundary(std::string &boundary,
		size_t &step, size_t &currStart, std::string &fileName)
{
	size_t	start;
	size_t	end;
	int		fd;
	std::string str = "";
	FileData fileData = {fileName, NULL, 0, 0};
	std::string path = _conf->getUploadDir();

	start = _body.find(boundary, currStart);
	if (start == std::string::npos)
	{
		std::cout << "la merde\n";
		step = 2;
		return (str);
	}
	if (!_body.substr(start, boundary.size() + 2)
			.compare(boundary + "--"))
	{
		step = 2;
		return (str);
	}
	switch (step)
	{
		case 0:
				start += (boundary.size() + 2);
				end = _body.find("\r\n\r\n", start);
				if (end != std::string::npos)
				{
					str = _body.substr(start, end - start);
					std::cout << YELLOW << "Get header Boundary:"
						<< std::endl << str << RESET << std::endl;
					currStart = end + 4;
					step = 1;
				}
			break ;
		case 1:
			end = _body.find(boundary, currStart);
			if (end == std::string::npos)
			{
				step = 2;
				break ;
			}
			step = 0;
			if (fileName.empty())
			{
				currStart = end;
				break ;
			}
			fileName = "";
			fileData.start = _body.data() + currStart;
			fileData.size = end - currStart - 2;
			fd = openFileUpload(path + "/" + fileData.fileName);
			if (fd >= 0)
				_filesUpload[fd] = fileData;
			currStart = end;
			break ;
		case 2:
			break ;
		default:
			break ;
	}
	return (str);
}

static std::string	getBoundaryFileName(std::string header)
{
	size_t		index;
	size_t		end;
	std::string	fileName = "";

	index = header.find("filename=\"");
	end = header.find("\"", index + 11);
	if (index != std::string::npos && end != std::string::npos)
		fileName = header.substr(index + 10, end - index - 10);
	std::cout << YELLOW << "In upload: path:" << "File Name: "
		<< fileName<< RESET << std::endl;
	return (fileName);
}

void	Response::uploadFile(std::map<std::string, std::string> const &headers)
{
	std::map<std::string, std::string>::const_iterator head;
	std::string path = _conf->getUploadDir();
	size_t	index;
	size_t	step = 0;
	size_t	currStart = 0;
	std::string boundary = "";
	std::string	fileName = "";
	std::string boundaryHeader = "";

	if (testAccess(path, 4))
	{
		std::cout << "body size: " << _body.size() << std::endl;
		head = headers.find("Content-Type");
		if (head != headers.end())
		{
			index = head->second.find("boundary=");
			if (index != std::string::npos)
			{
				boundary = head->second.substr(index + 9);
				/* while boundary end*/
				while (step != 2)
				{
					//std::cout << GREEN << "Body: " << _body.data() + currStart
						//<< RESET << std::endl;
					boundaryHeader = handleBoundary(boundary, step, currStart,
							fileName);
					if (!boundaryHeader.empty())
						fileName = getBoundaryFileName(boundaryHeader);
				}
			}
		}
		if (_filesUpload.empty())
		{
			createError(500);
			return ;
		}
		std::map<int, FileData>::iterator it;
		for (it = _filesUpload.begin(); it != _filesUpload.end(); it++)
		{
			FileData tmp = it->second;
			std::cout << GREEN << "File Name: " << tmp.fileName
				<< std::endl << "File size: "
				<< tmp.size << RESET << std::endl;
			addFdToCluster(it->first, POLLOUT);
			
			std::map<int, FileData>::iterator it2 = it;
			++it2;
			if (it2 == _filesUpload.end())
				return ;
		}
	}
	createError(400);
	return ;
}
