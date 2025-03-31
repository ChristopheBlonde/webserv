/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Requests.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/21 10:46:49 by cblonde           #+#    #+#             */
/*   Updated: 2025/03/31 12:47:04 by cblonde          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Requests.hpp"
#include "Client.hpp"

Requests::Requests(std::string str, Client &client)
	: _type(UNKNOWN),
	  _client(client)
{
	initMimeTypes(_mimeTypes);
	this->parse(str);
	return ;
}

Requests::Requests(Requests const &src) : _client(src._client)
{
	*this = src;
	return ;
}

Requests::~Requests(void)
{
	return ;
}

Requests	&Requests::operator=(Requests const &rhs)
{
	if (this != &rhs)
	{
		this->_headers = rhs._headers;
		this->_host = rhs._host;
		this->_path = rhs._path;
		this->_type = rhs._type;
		this->_protocol = rhs._protocol;
		this->_query = rhs._query;
		this->_body	= rhs._body;
		this->_mimeTypes = rhs._mimeTypes;
		this->_fileName = rhs._fileName;
		this->_port = rhs._port;
	}
	return (*this);
}

static void initMethod(std::string str, t_rqType &type)
{
	std::string key[3] = {"GET","POST","DELETE"};
	t_rqType	types[3] = {GET, POST, DELETE};

	type = UNKNOWN;
	for (size_t i = 0; i < 3; i++)
	{
		if (str == key[i])
			type = types[i];
	}
	return ;
}

void	Requests::handlePath(void)
{
	size_t		index;
	size_t		j;
	std::string	tmp;

	handleBadPath(_path);
	_requestUri = _path;
	index = _path.find("?");
	if (index != std::string::npos)
	{
		_query = _path.substr(index + 1);
		_path = _path.substr(0, index);
	}
	else
		_query = "";
	_documentUri = _path;
	index = 0;
	while (index != std::string::npos && _path.size() > 1)
	{
			index = _path.find("/", index);
			index++;
			j = _path.find("/", index);
			if (j != std::string::npos)
			{
				_fileName = _path.substr(index, j - index);
				_pathInfo = _path.substr(j + 1);
			}
			else
				_fileName = _path.substr(index);
			if (_fileName.empty())
				break ;
			j = _fileName.find_last_of(".");
			if (j != std::string::npos)
				tmp = _mimeTypes[std::string(_fileName.substr(j + 1))];
			if (tmp != "")
			{
				if (index > 1)
					_path = _path.substr(0, index - 1);
				if (_path.empty())
					_path = "/";
				break ;
			}
			else
			{
				_fileName = "";
				_pathInfo = "";
			}
	}
	std::cout << CYAN << "Path: " << _path << " file: " << _fileName
		<< " pathInfo: " << _pathInfo << RESET << std::endl;
}

void	Requests::handleHost(void)
{
	size_t		index;
	char const	*tmp;
	char		*end;

	_host = _headers["Host"];
	index = _host.find(":");
	if (index != std::string::npos)
	{
		tmp = _host.substr(index + 1).data();
		_host = _host.substr(0, index);
		_port = static_cast<int>(strtol(tmp, &end, 10));
	}
	else
		_port = 8080;
}

static void	initHeaders(std::string str,
		std::map<std::string,std::string> &headers)
{
	std::string	key;
	size_t		index;
	std::string	line;

	trim(str);
	std::stringstream ss(str);
	while (getline(ss, line))
	{
		if (line.empty())
			return ;
		std::pair<std::string, std::string> pair;
		if ((index = line.find(":")) == std::string::npos)
			pair.first = line.substr(0);
		else
		{
			pair.first = line.substr(0, index);
			pair.second = line.substr(index + 1, line.size() - index - 2);
		}
		formatHeader(pair.first);
		trim(pair.second);
		if (!pair.first.empty())
			headers.insert(pair);
	}
}

void	Requests::parse(std::string str)
{
//	std::cout << CYAN << "Request: " << str << RESET << std::endl;
	std::cout << "request size: " << str.size() << std::endl;
	std::stringstream	ss(str);
	std::string			word;
	std::string			line;
	size_t				index;

//	std::cout << "received request:\n|||\n" << str << "|||\n";
	getline(ss, line);
	std::stringstream ssLine(line);
	ssLine >> word >> _path >> _protocol;
	if (_protocol.compare("HTTP/1.0") && _protocol.compare("HTTP/1.1"))
	{
		std::cerr << RED << "Error: Protocol: unknow" << RESET << std::endl;
		return ;
	}
	initMethod(word, _type);
	if (_type == UNKNOWN)
	{
		std::cerr << RED << "Unknow method or not implement yet."
			<< RESET << std::endl;
		return ;
	}
	handlePath();
	while (getline(ss, line))
	{
		trim(line);
		if (line.empty())
			break ;
		initHeaders(line, _headers);
	}

	handleHost();
	index = str.find("\r\n\r\n");
	if (index != std::string::npos)
		_body = str.substr(index +4);
	return ;
}
