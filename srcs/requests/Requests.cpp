/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Requests.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/21 10:46:49 by cblonde           #+#    #+#             */
/*   Updated: 2025/03/20 20:38:52 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Requests.hpp"

Requests::Requests(void)
{
	return ;
}

Requests::Requests(std::string str)
{
	initMimeTypes(_mimeTypes);
	this->parse(str);
	return ;
}

Requests::Requests(Requests const &src)
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

	type = UNKNOW;
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
	std::string	tmp;

	index = _path.find("?");
	if (index != std::string::npos)
	{
		_query = _path.substr(index + 1);
		_path = _path.substr(0, index);
	}
	else
		_query = "";
	index = _path.find_last_of("/\\");
	if (index != std::string::npos)
	{
			_fileName = _path.substr(index + 1);
			index = _fileName.find_last_of(".");
			if (index != std::string::npos)
				tmp = _mimeTypes[std::string(_fileName.substr(index + 1))];
			if (tmp != "")
				_path = _path.substr(0, _path.find_last_of("/\\"));
			else
				_fileName = "";
	}
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
		trim(pair.first);
		trim(pair.second);
		if (!pair.first.empty())
			headers.insert(pair);
	}
}

void	Requests::parse(std::string str)
{
	std::stringstream	ss(str);
	std::string word;
	std::string	line;

	getline(ss, line);
	std::stringstream ssLine(line);
	ssLine >> word >> _path >> _protocol;
	if (_protocol.compare("HTTP/1.0") && _protocol.compare("HTTP/1.1"))
	{
		std::cout << RED << "Error: Protocol: unknow" << RESET << std::endl;
		return ;
	}
	initMethod(word, _type);
	if (_type == UNKNOW)
	{
		std::cout << RED << "Unknow method or not implement yet."
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
	std::cout << RED << "Request host: " << _host << "port: " << _port
		<< " path: " << _path << "file: " << _fileName << RESET << std::endl;
	return ;
}

std::string	Requests::getProtocol(void) const
{
	return (_protocol);
}

std::string	Requests::getPath(void) const
{
	return (_path);
}

std::map<std::string,std::string>	Requests::getHeaders(void) const
{
	return (_headers);
}

std::string	Requests::getHost(void) const
{
	return (_host);
}

std::string	Requests::getType(void) const
{
	std::string key[3] = {"GET","POST","DELETE"};
	t_rqType	types[3] = {GET, POST, DELETE};
	for (size_t i = 0; i < 3; i++)
	{
		if (_type == types[i])
			return (key[i]);
	}
	return ("");
}

std::string	Requests::getBody(void) const
{
	return (_body);
}

std::string	Requests::getQuery(void) const
{
	return (_query);
}

std::string Requests::getFileName(void) const
{
	return (_fileName);
}

int			Requests::getPort(void) const
{
	return (_port);
}
