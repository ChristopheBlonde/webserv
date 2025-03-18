/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 11:15:20 by cblonde           #+#    #+#             */
/*   Updated: 2025/03/18 16:27:43 by cblonde          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Response.hpp>
#include <ErrorPages.hpp>
#include <Cgi.hpp>

Response::Response(void)
{
	return ;
}

Response::Response(Requests const &req) : _path(req.getPath())
{
	std::cout << GREEN << req.getBody() << RESET <<std::endl;
	this->_headerSend = false;
	this->_sizeSend = 0;
	this->_fileFd = -1;
	this->_protocol = req.getProtocol();
	this->_host = req.getHost();
	this->_path = req.getPath();
	this->_port = req.getPort();
	this->_fileName = req.getFileName();
	this->_cgi = false;
	initMimeTypes(_mimeTypes);
	initResponseHeaders(_headers);
	
	/* check headers request */

	isReferer(req);
	handleFile();
//if (_path.find_last_of(".") != std::string::npos
//		&& (std::string(_path.substr(_path.size() - 3))) == ".js")
//{
//	std::cout << CYAN << "CGI test file: " << _path << RESET << std::endl;
//	_cgi = true;
//	Cgi	test(req);
//	_response = test.execScript();
//}
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

void	Response::isReferer(Requests const &req)
{
	std::map<std::string,std::string>	header(req.getHeaders());
	size_t								index;
	std::string							path;

	if (!header["Referer"].empty())
	{
		path = header["Referer"];
		index = path.find(header["Host"]);
		path = path.substr(index + header["Host"].size());
		index = _path.find(path);
		if (index == std::string::npos || index != 0)
			_path = path + _path;
	}
}

void	Response::handleFile(void)
{
	/* check cgi */

	/* no cgi */
	_fileFd = openDir(_path, _fileName);
	if (_fileFd == -1)
	{
		createError(404);
	}
	/* add fd to poll*/
	std::cout << RED << "FD OPEN BY OPEN DIR: " << _fileFd
		<< RESET << std::endl;
}

static std::string	getNameError(int stat)
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
		content = ERROR_PAGE(getNameError(stat), getContentError(stat),
				to_string(stat));
	else
		content = AutoIndex::generate(_path.data(), "localhost", 8080);
	result += getNameError(stat) + "\n";
	//_contentLen = content.size();
	result += "Content-Type: text/html; charset=UTF-8\nContent-Length: "
		+ to_string(content.size()) + "\n"
		+ (_autoIndex ? "Connection: close\n" : "")
		+ "Server: webserv 1.0\n"
		+"\r\n";
	result += content;
	_resSize = result.size();
	_response = result;
	send(getSocket(), _response.c_str(), _response.size(), 0);
	return ;
}

void	Response::createResponse(void)
{
	std::map<std::string,std::string>::iterator it;
	/* Create first line */
	_response = _protocol + " 200 OK\n";
	/* Create headers */
	_headers["Content-Length"] += to_string(_buffer.size());
	_headers["Content-Type"] += !_cgi
		? _mimeTypes[getFileType(_path + _fileName)]
		: "text/html; charset=UFT-8";
	/* join all */
	for (it = _headers.begin(); it != _headers.end(); it++)
		_response += (it->second + "\r\n");
	_response += "\r\n";

	//_response += _fileContent;
	//_resSize = _response.size();
	std::cout << CYAN << "Header ready" << RESET << std::endl;
	_response.insert(_response.end(), _buffer.begin(), _buffer.end());
	send(getSocket(), _response.c_str(), _response.size(), 0);
	//setFileStatus(fd);
	return ;
}

bool	Response::handleInOut(struct pollfd &fd)
{
	int				readByte;
	int				sentByte;
	unsigned char	buffer[BUFFER_SIZE];

	//std::cout << GREEN << "In handleInOut: fd: " << fd.fd
	//	<< " envent: " << fd.events << " revent: " << fd.revents
	//	<< RESET << std::endl;
	if (fd.revents & POLL_IN)
	{
		if (fd.fd == _fileFd)
		{
			readByte = read(fd.fd, buffer, BUFFER_SIZE - 1);
			if (readByte <= 0)
			{
				createResponse();
				_fileFd = -1;
				return (false);
			}
			_buffer.insert(_buffer.end(), buffer, buffer + readByte);
		}
	}
		if (_buffer.empty() && fd.fd == -1)
			return (false);
	if (fd.revents & POLL_OUT)
	{
		if (!_headerSend)
		{
			if (!_response.empty())
			{
				sentByte = send(fd.fd, _response.c_str(), BUFFER_SIZE, 0);
				_response.erase(0, sentByte);
			}
			else
				_headerSend = true;
		}
		if (!_buffer.empty())
		{
			sentByte = send(fd.fd, _buffer.data(), BUFFER_SIZE, 0);
			if (sentByte > 0)
				_buffer.erase(_buffer.begin(), _buffer.begin() + sentByte);
		}
	}
	return (true);
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

//void	Response::setFileContent(std::string &str)
//{
//	_fileContent = str;
//}

void	Response::setSocket(int const socket)
{
	_socket = socket;
}
