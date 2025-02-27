/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 11:15:20 by cblonde           #+#    #+#             */
/*   Updated: 2025/02/27 16:09:14 by cblonde          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Response.hpp>

Response::Response(void)
{
	return ;
}

Response::Response(Requests const &req)
{
	this->_protocol = req.getProtocol();
	this->_path = req.getPath();
	initMimeTypes(_mimeTypes);
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
	std::cout << "path: " << _path << std::endl;
	std::pair<int,std::string> content;
	content = openDir(_path);
	std::cout << GREEN << content.second << std::endl << RESET;
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
