/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Requests.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/21 10:46:49 by cblonde           #+#    #+#             */
/*   Updated: 2025/04/04 18:16:20 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Requests.hpp"
#include "Cluster.hpp"
#include "Client.hpp"

Requests::Requests(std::string &str, Client &client, Cluster *c, int sockFd)
	: _client(client)
{
	error = 200;
	initMimeTypes(_mimeTypes);
	fd = sockFd;
	this->parse(str, c);
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
	}
	return (*this);
}

static void initMethod(std::string str, t_rqType &type)
{
	std::string key[3] = {"GET", "POST", "DELETE"};
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
	size_t		queryPos;

	queryPos = _path.find("?");
	if (queryPos != std::string::npos)
	{
		_query = _path.substr(queryPos + 1);
		_path = handleBadPath(urlDecode(_path.substr(0, queryPos)));
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

static int	initHeader(std::string str,
		std::map<std::string,std::string> &headers)
{
	std::string				key;
	size_t					index;
	std::pair<std::string, std::string>	pair;

	if ((index = str.find(":")) == std::string::npos)
		return 1;
	pair.first = str.substr(0, index);
	pair.second = str.substr(index + 1);
	formatHeader(pair.first);
	trim(pair.second);
	if (pair.first.empty())
		return 1;
	headers.insert(pair);
	return 0;
}

void	Requests::handleFile(std::string start, std::string alias)
{
	size_t		lastSlash = 0;
	size_t		slash;

	_path = _path.substr(start.size());
	slash = _path.find("/", 1);
	while (slash != std::string::npos)
	{
		if (testAccess(alias + _path.substr(0, slash), DIRACCESS))
		{
			lastSlash = slash;
			slash = _path.find("/", lastSlash + 1);
			continue;
		}
		_fileName = _path.substr(lastSlash + 1, slash - lastSlash - 1);
		_pathInfo = _path.substr(slash);
		_path = alias + _path.substr(0, lastSlash + 1);
		return;
	}
	if ((lastSlash < _path.size() - 1)
		&& testAccess(alias + _path, DIRACCESS))
	{
		if (error == 200)
			error = (_type == GET) ? 301 : 308;
		_path = start + _path + "/";
		if (!_query.empty())
			_path += "?" + _query;
		return;
	}
	_fileName = _path.substr(lastSlash + 1);
	_path = alias + _path.substr(0, lastSlash + 1);
}

void	Requests::parse(std::string str, Cluster *c)
{
	std::cout << CYAN << "raw request: |||" << RESET << std::endl
		<< str << CYAN << "|||" << RESET << std::endl;
	std::stringstream	ss(str);
	std::string			word;
	std::string			line;
	size_t				index;

	getline(ss, line);
	std::stringstream ssLine(line);
	ssLine >> word >> _path >> _protocol;
	if ((_protocol.compare("HTTP/1.0") && _protocol.compare("HTTP/1.1"))
		|| line[line.size() - 1] != '\r')
	{
		error = 400;
		_protocol = "HTTP/1.1";
	}
	initMethod(word, _type);
	if (_type == UNKNOWN)
		error = 400;
	while (getline(ss, line))
	{
		if (line[line.size() - 1] != '\r')
		{
			error = 400;
			return;
		}
		line.erase(line.size() - 1);
		if (line.empty())
			break ;
		trim(line);
		if (initHeader(line, _headers))
		{
			error = 400;
			return;
		}
	}
	_host = _headers["Host"];
	if (_host == "")
	{
		error = 400;
		return;
	}
	handlePath();
	_conf = &c->getRoute(c->getServer(fd, _host.substr(0, _host.find(":"))), _path);

	std::string	mount = _conf->getMount();

	if (!testAccess(mount, DIRACCESS))
	{
		std::cout << mount <<" wtf\n";
		error = 404;
		return;
	}
	handleFile(_conf->getAliasedPart(), mount);
	index = str.find("\r\n\r\n");
	if (index != std::string::npos)
	{
		str.erase(0, index + 4);
		_body = str;
	}
	return ;
}
