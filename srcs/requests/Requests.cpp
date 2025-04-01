/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Requests.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/21 10:46:49 by cblonde           #+#    #+#             */
/*   Updated: 2025/04/01 05:18:38 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Requests.hpp"
#include "Cluster.hpp"
#include "Client.hpp"

Requests::Requests(std::string &str, Client &client, Cluster *c, int fd)
	: _client(client)
{
	error = 200;
	initMimeTypes(_mimeTypes);
	this->parse(str, c, fd);
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

	index = _path.find("?");
	if (index != std::string::npos)
	{
		_query = _path.substr(index + 1);//needs to be decoded after parsing
		_path = handleBadPath(urlDecode(_path.substr(0, index)));
		_requestUri = handleBadPath(_path + "?" + _query);
	}
	else
	{
		_query = "";
		_path = handleBadPath(urlDecode(_path));
		_requestUri = _path;
	}
	_documentUri = _path;
}

void	Requests::handleFile(void)
{
	size_t		index;

	index = _path.find_last_of("/");
	if (index == _path.size() - 1)
		return;
	if (testAccess(_conf->getRoot() + _path, DIRACCESS))
	{
		if (error == 200)
		{
			if (_type == GET)
				error = 301;
			else
				error = 308;
		}
		_path += "/";
		return;
	}
	_fileName = _path.substr(index + 1);
	_pathInfo = _path.substr(1, index);
	_path = _path.substr(0, index + 1);
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

void	Requests::parse(std::string str, Cluster *c, int fd)
{
	std::cout << CYAN << "raw request: |||" << RESET << str << CYAN << "|||\n" << RESET;
	std::stringstream	ss(str);
	std::string			word;
	std::string			line;
	size_t				index;

	getline(ss, line);
	std::stringstream ssLine(line);
	ssLine >> word >> _path >> _protocol;
	if (_protocol.compare("HTTP/1.0") && _protocol.compare("HTTP/1.1"))
	{
		error = 400;
		_protocol = "HTTP/1.1";
	}
	initMethod(word, _type);
	if (_type == UNKNOWN)
		error = 400;
	while (getline(ss, line))
	{
		trim(line);
		if (line.empty())
			break ;
		initHeaders(line, _headers);
	}
	handleHost();
	handlePath();
	_conf = &c->getRoute(c->getServer(fd, _host), _path);
	handleFile();
	index = str.find("\r\n\r\n");
	if (index != std::string::npos)
	{
		str.erase(0, index + 4);
		_body = str;
	}
	return ;
}
