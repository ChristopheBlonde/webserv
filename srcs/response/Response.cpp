/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 11:15:20 by cblonde           #+#    #+#             */
/*   Updated: 2025/03/13 17:05:43 by cblonde          ###   ########.fr       */
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
		this->_contentLen = rhs._contentLen;
		this->_status = rhs._status;
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
	int	fd;
	/* check cgi */

	/* no cgi */
	fd = openDir(_path, _fileName);
	/* add fd to poll*/
	_fileStatus = 0;
	std::cout << RED << "FD OPEN BY OPEN DIR: " << fd << RESET << std::endl;
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
	_contentLen = content.size();
	result += "Content-Type: text/html; charset=UTF-8\nContent-Length: "
		+ to_string(_contentLen) + "\n" + (_autoIndex ? "Connection: close\n" : "")
		+ "Server: webserv 1.0\n"
		+"\r\n";
	result += content;
	_resSize = result.size();
	_response = result;
	return ;
}

void	Response::createResponse(void)
{
	/* check conf index */
	/* check path to file */
	/* check extention */
	/* check in request accept file correspondance */
	/* get content */
	/* build header */
	/* build result */
//std::pair<int,std::string> content;
//std::map<std::string,std::string>::iterator it;
//if (!_cgi)
//	content = openDir(_path);
//else
//{
//	content.second = _response;
//	content.first = _response.size();
//}
//if (!content.first)
//{
//	createError(404);
//	return ;
//}
///* Create first line */
//_response = _protocol + " 200 OK\n";
///* Create headers */
//_headers["Content-Length"] += to_string(content.first);
//_headers["Content-Type"] += !_cgi ? _mimeTypes[getFileType(_path)]
//	: "text/html; charset=UFT-8";
//_headers["Set-Cookie"] += "name=Chris";
///* join all */
//for (it = _headers.begin(); it != _headers.end(); it++)
//	_response += (it->second + "\n");
//_response += "\r\n";
//_response += content.second;
//_resSize = _response.size();
	return ;
}

std::string	Response::getResponse(void)
{
	return (_response);
}

size_t	Response::getResSize(void)
{
	return (_resSize);
}
