/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 11:15:20 by cblonde           #+#    #+#             */
/*   Updated: 2025/03/27 08:46:44 by cblonde          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Response.hpp>

Response::Response(void)
{
	return ;
}

Response::Response(Requests const &req)
{
	std::map<std::string, std::string> const &headers = req.getHeaders();

	this->_body = req.getBody();
	this->_headerSent = false;
	this->_headerReady = false;
	this->_sizeSend = 0;
	this->_fileFd = -1;
	this->_protocol = req.getProtocol();
	this->_host = req.getHost();
	this->_path = req.getPath();
	this->_port = req.getPort();
	this->_fileName = req.getFileName();
	this->_conf = &req.getConf();
	this->_cgi = _conf->getCgi().empty() ? false : true;
	this->_autoIndex = _conf->getAutoindex();
	this->_status = 200;
	initMimeTypes(_mimeTypes);
	initResponseHeaders(_headers);
	
	/* check headers request */

	checkConnection(headers);
	if (!checkMethod(req.getType()))
		return ;
	if (!checkContentLen(headers))
		return ;
	isReferer(headers);
	handleFile(req);
	return ;
}

Response::Response(Response const &src)
{
	*this = src;
	return ;
}

Response::~Response(void)
{
	return ;
}

Response	&Response::operator=(Response const &rhs)
{
	if (this != &rhs)
	{
		this->_protocol = rhs._protocol;
		this->_body = rhs._body;
		this->_path = rhs._path;
		this->_host = rhs._host;
		this->_port = rhs._port;
		this->_headers = rhs._headers;
		this->_autoIndex = rhs._autoIndex;
		this->_response = rhs._response;
		this->_resSize = rhs._resSize;
	}
	return (*this);
}

void	Response::checkConnection(std::map<std::string,
		std::string> const &headers)
{
	std::map<std::string, std::string>::const_iterator it;

	it = headers.find("Connection");
	if (it != headers.end())
		_headers["Connection"] += it->second;
}


bool	Response::checkMethod(std::string method)
{
	std::cout << RED << "METHOD: " << method << RESET << std::endl;
	std::set<std::string> &methods(_conf->getAcceptedMethods());
	std::set<std::string>::iterator it;
	for (it = methods.begin(); it != methods.end(); it++)
		std::cout << GREEN << "Method in conf: " << *it << RESET <<std::endl;
	it = methods.find(method);
	if (it == methods.end())
	{
		createError(405);
		return (false);
	}
	return (true);
}

bool	Response::checkContentLen(std::map<std::string,
		std::string> const &headers)
{
	std::map<std::string, std::string>::const_iterator it;
	long	contentLen;

	it = headers.find("Content-Length");
	if (it != headers.end())
	{
		contentLen = strtol(it->second.data(), NULL, 10);
		if (12500000 < contentLen)
		{
			createError(413);
			return (false);
		}
	}
	return (true);
}

void	Response::isReferer(std::map<std::string, std::string> const &headers)
{
	std::map<std::string, std::string>::const_iterator it;

	it = headers.find("Referer");
	std::string referer = it != headers.end() ? it->second : "";
	if (!referer.empty())
	{
		size_t index = referer.find(to_string(_port));
		referer = referer.substr(index + to_string(_port).size());
		handleBadPath(referer);
	}
	std::string routePath = _conf->getName();
	std::cout << RED << "pathRoute: " << routePath << RESET <<std::endl;
	if (routePath == _path || routePath == referer)
		routePath = _conf->getRoot();
	else
		routePath = _path;

	if (*(routePath.begin() + routePath.size() - 1) == '/')
		routePath = routePath.substr(0, routePath.size() - 1);
	if (!referer.empty())
		_path = routePath + _path;
	else
		_path = routePath;
}

void	Response::handleFile(Requests const &req)
{
		/* upload */
	if ((req.getType() == "POST" || req.getType() == "DELETE")
			&& !_conf->getUploadDir().empty())
	{
		uploadFile(req.getHeaders());
	}
	else if (_cgi)
	{
		/* cgi */
		std::map<std::string, std::string> cgi(_conf->getCgi());
		for (std::map<std::string, std::string>::iterator it = cgi.begin();
				it != cgi.end(); it++)
		{
			if (!testAccess(it->second, 0) || !testAccess(it->second, 3))
			{
				createError(400);
				return ;
			}
		}
		Cgi cgiObj(req);
		std::string result = cgiObj.execScript();
		_buffer.insert(_buffer.begin(), result.begin(), result.end());
		createResponseHeader();
	}
	else
	{
		/* no cgi */
		_fileFd = openDir(_path, _fileName, _conf->getIndex());
		getStatFile();
		if (_fileFd == -1)
			createError(404);
		else if (_fileFd == -2)
			createError(400);
		/* add fd to poll*/
		std::cout << RED << "FD OPEN BY OPEN DIR: " << _fileFd
			<< RESET << std::endl;
	}
}

static std::string	getResponseTypeStr(int stat)
{
	std::map<int,std::string> names;

	names[200] = "OK";
	names[400] = "Bad Request";
	names[403] = "Forbidden";
	names[404] = "Not Found";
	names[405] = "Method Not Allowed";
	names[410] = "Gone";
	names[413] = "Content Too Large";
	names[500] = "Internal Server Error";

	return (names[stat]);
}

static std::string	getContentError(int stat)
{
	std::map<int,std::string> content;

	content[200] = "OK";
	content[400] = "the server would not process the request due to something \
the server considered to be a client error";
	content[403] = "Insufficient permissions to a resource or action";
	content[404] = "Page not found";
	content[405] = "Target resource doesn't support this method";
	content[410] = "Target resource is no longer available";
	content[413] = "Request entity was larger than limits defined by server";
	content[500] = "Server encountered an unexpected condition that prevented \
it from fulfilling the request";

	return (content[stat]);
}

void	Response::createError(int stat)
{
	std::string			content;
	std::string 		result;

	result = _protocol + " " + to_string(stat) + " ";
	if (!_autoIndex)
		content = ERROR_PAGE(getResponseTypeStr(stat), getContentError(stat),
				to_string(stat));
	else
		content = AutoIndex::generate(_path.data(), _host, _port);
	result += getResponseTypeStr(stat) + "\r\n";
	result += "Content-Type: text/html; charset=UTF-8\r\nContent-Length: "
		+ to_string(content.size()) + "\r\n"
		+ (_autoIndex
				? "Connection: close\r\n"
				: _headers["Connection"] + "\r\n")
		+ "Server: webserv 1.0\n"
		+"\r\n";
	result += content;
	_resSize = result.size();
	_response = result;
	_headerReady = true;
	return ;
}

void	Response::createResponseHeader(void)
{
	std::map<std::string,std::string>::iterator it;

	/* Create first line */
	_response = _protocol
		+ to_string(_status)
		+ getResponseTypeStr(_status)
		+ "\r\n";
	/* Create headers */

	_headers["Content-Length"] += to_string(_buffer.size());
	_headers["Content-Type"] += !_cgi
		? _mimeTypes[getFileType(_fileName)]
		: "text/html; charset=UFT-8";
	/* join all */
	for (it = _headers.begin(); it != _headers.end(); it++)
		_response += (it->second + "\r\n");
	_response += "\r\n";

	_headerReady = true;
	std::cout << CYAN << "Header ready" << RESET << std::endl;
	return ;
}

bool	Response::handleInOut(struct pollfd &fd)
{
	int				readByte;
	int				sentByte;
	unsigned char	buffer[FILE_BUFFER_SIZE];

//	std::cout << GREEN << "In handleInOut: fd: " << fd.fd
//		<< " envent: " << fd.events << " revent: " << fd.revents
//		<< RESET << std::endl;
	if (fd.revents & POLLIN)
	{
		if (fd.fd == _fileFd)
		{
			readByte = read(fd.fd, buffer, FILE_BUFFER_SIZE - 1);
			if (readByte <= 0)
			{
				createResponseHeader();
				_fileFd = -1;
				return (false);
			}
			_buffer.insert(_buffer.end(), buffer, buffer + readByte);
		}
	}
		if (_buffer.empty() && fd.fd == -1)
			return (false);
	if (fd.revents & POLLOUT)
	{
		if (!_headerSent && _headerReady)
		{
			if (!_response.empty())
			{
				sentByte = send(fd.fd, _response.c_str(),
						_response.size() < FILE_BUFFER_SIZE
						? _response.size() : FILE_BUFFER_SIZE, 0);
				_response.erase(0, sentByte);
			}
			else
				_headerSent = true;
		}
		if (!_buffer.empty() && _headerSent)
		{
			sentByte = send(fd.fd, _buffer.data(), _buffer.size() < FILE_BUFFER_SIZE
					? _buffer.size() : FILE_BUFFER_SIZE, 0);
			if (sentByte > 0)
				_buffer.erase(_buffer.begin(), _buffer.begin() + sentByte);
		}
		if (_buffer.empty() && _headerSent)
			return (false);
	}
	return (true);
}

void	Response::getStatFile(void)
{
	struct stat res;
	int			status;
	time_t		ttime;
	struct tm	*tmTime;
	char		buffer[1024];
	size_t		size;

	status = stat(std::string(_path + "/" + _fileName).c_str(), &res);
	if (status)
	{
		std::cout << RED << "Error: stat: " << strerror(errno)
			<< RESET << std::endl;
		return ;
	}
	#ifdef __linux__
		ttime = res.st_mtim.tv_sec;
	#else
		ttime = res.st_mtimespec.tv_sec;
	#endif
	tmTime = gmtime(&ttime);
	strftime(buffer, 1024, "%a, %d %b %Y %T GMT", tmTime);
	std::cout << CYAN << "Stat: Last-Modified: " << buffer << RESET << std::endl;
	_headers["Last-Modified"] += buffer;
	ttime = time(NULL);
	tmTime = gmtime(&ttime);
	strftime(buffer, 1024, "%a, %d %b %Y %T GMT", tmTime);
	std::cout << CYAN << "Stat: Date: " << buffer << RESET << std::endl;
	_headers["Date"] += buffer;
	size = res.st_size;
	_buffer.reserve(size * sizeof(unsigned char));
	std::cout << CYAN << "Stat: Size: " << size << RESET << std::endl;
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
					std::cout << GREEN << "Body: " << _body.data() + currStart
						<< RESET << std::endl;
					boundaryHeader = handleBoundary(boundary, step, currStart,
							fileName);
					if (!boundaryHeader.empty())
						fileName = getBoundaryFileName(boundaryHeader);
				}
				std::map<int, FileData>::iterator it;
				for (it = _filesUpload.begin(); it != _filesUpload.end(); it++)
				{
					FileData tmp = it->second;
					std::cout << GREEN << "File Name: " << tmp.fileName
						<< std::endl << "File size: "
						<< tmp.size << RESET << std::endl;
				}
				return ;
			}
		}
	}
	createError(400);
	return ;
}

std::string	Response::getResponse(void) const
{
	return (_response);
}

size_t	Response::getResSize(void) const
{
	return (_resSize);
}

int	Response::getFileFd(void) const
{
	return (_fileFd);
}

int	Response::getSocket(void) const
{
	return (_socket);
}

void	Response::setResponse(std::string str)
{
	_response = str;
}

void	Response::setSocket(int const socket)
{
	_socket = socket;
}
