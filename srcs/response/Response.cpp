/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 11:15:20 by cblonde           #+#    #+#             */
/*   Updated: 2025/02/28 14:47:46 by cblonde          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Response.hpp>

Response::Response(void)
{
	return ;
}

Response::Response(Requests const &req) : _path(req.getPath())
{
	this->_protocol = req.getProtocol();
	initMimeTypes(_mimeTypes);
	initResponseHeaders(_headers);
	
	/* check headers request */
	std::map<std::string,std::string> header(req.getHeaders());
	if (!header["Referer"].empty())
	{
		size_t		index;
		std::string	path;

		path = header["Referer"];
		index = path.find(header["Host"]);
		path = path.substr(index + header["Host"].size());
		index = _path.find(path);
		std::cout << CYAN << "path: " << path
			<< " index: " << index << std::endl << RESET;
		if (index == std::string::npos || index != 0)
			_path = path + _path;
	}
	std::cout << RED << "\tPATH: " << _path << std::endl << RESET;
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
		this->_headers = rhs._headers;
		this->_contentLen = rhs._contentLen;
		this->_status = rhs._status;
		this->_autoIndex = rhs._autoIndex;
		this->_response = rhs._response;
		this->_resSize = rhs._resSize;
	}
	return (*this);
}

static std::string	getNameError(std::pair<int, std::string> content)
{
	std::string 			str;
	std::string				result;
	size_t					start;
	size_t					end;

	str = content.second;
	start = str.find("<title>") + 7;
	end = str.find("</title>");
	result = str.substr(start, end - start);
	return (result);
}

void	Response::createError(int stat)
{
	std::stringstream			ss;
	std::pair<int, std::string>	content;
	std::string 				status;
	std::string 				path;
	std::string 				result;

	ss << stat;
	ss >> status;
	result = _protocol + " " + status + " "; 
	path = "./srcs/default_pages/default_" + status + ".html";
	content = getFile(path);
	status = getNameError(content);
	result += status + "\n";
	ss.str(std::string());
	ss.clear();
	ss << content.first;
	result += "Content-Type: text/html; charset=UTF-8\nContent-Length: "
		+ ss.str() + "\n\r\n" + content.second;
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
	std::stringstream ss;
	std::pair<int,std::string> content;
	std::map<std::string,std::string>::iterator it;
	content = openDir(_path);
//	std::cout << GREEN << content.second << std::endl << RESET;
	if (!content.first)
		createError(404);
	/* Create first line */
	_response += _protocol + " 200 OK\n";
	/* Create headers */
	ss << content.first;
	_headers["Content-Length"] += ss.str();
	_headers["Content-Type"] += _mimeTypes[getFileType(_path)];
	/* join all */
	for (it = _headers.begin(); it != _headers.end(); it++)
		_response += (it->second + "\n");
	_response += "\r\n";
	_response += content.second;
	_resSize = _response.size();
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
