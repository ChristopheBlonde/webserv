/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Requests.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/21 10:46:49 by cblonde           #+#    #+#             */
/*   Updated: 2025/02/27 13:07:13 by cblonde          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Requests.hpp>

Requests::Requests(void)
{
	return ;
}

Requests::Requests(std::string str)
{
	this->_type = UNKNOW;
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
	}
	return (*this);
}

static void initMethod(std::string str, t_rqType &type)
{
	std::string key[3] = {"GET","POST","DELETE"};
	t_rqType	types[3] = {GET, POST, DELETE};
	for (size_t i = 0; i < 3; i++)
	{
		if (str == key[i])
			type = types[i];
	}
	return ;
}

static void initQuery(std::string &str,
		std::vector<std::pair<std::string,std::string> > &query)
{
	size_t		index;
	std::string	qstr;
	std::string	tmp;

	index = str.find("?");
	if (index == std::string::npos)
		return ;
	if (str[index + 1] == '\0')
		return ;
	qstr = str.substr(index + 1);
	str = str.substr(0, index);
	std::stringstream query_str(qstr);
	while (getline(query_str, tmp, '&'))
	{
		std::pair<std::string, std::string> pair;
		index = tmp.find("=");
		if (index == std::string::npos)
		{
			pair.first = tmp;
			pair.second = "";
		}
		else
		{
			pair.first = tmp.substr(0, index);
			pair.second = tmp.substr(index + 1);
		}
		if (!pair.first.empty())
			query.push_back(pair);
	}
	return ;
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
			pair.second = line.substr(index + 1);
		}
		trim(pair.first);
		toUpper(pair.first);
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
	initQuery(_path, _query);
	
	/* Print variables*/
	std::cout << CYAN << "Method: " << (_type == 0 ? "GET" : _type == 1
			? "POST" : _type == 2 ? "DELETE" : "UNKNOW") << " Path: "
		<< _path<< " Protocol: " << _protocol << RESET << std::endl;
	/* Print querys */
	std::vector<std::pair<std::string,std::string> >::iterator it;
	for (it = _query.begin(); it != _query.end(); it++)
	{
		std::cout << RED << "key: " << it->first << CYAN << " value: "
			<< it->second << std::endl << RESET;
	}
	while (getline(ss, line))
	{
		trim(line);
		if (line.empty())
			break ;
		initHeaders(line, _headers);
	}
	_host = _headers["HOST"];

	/* Print headers*/
	std::map<std::string,std::string>::iterator ite;
	std::cout << GREEN << "headers contains:" << std::endl;
	for (ite=_headers.begin(); ite!=_headers.end(); ++ite)
		std::cout << GREEN << ite->first << " => " << ite->second << RESET << std::endl;

	std::cout << RED << "Try get host: " << _host << RESET << std::endl;

	/* test trim function */
	std::string test("   tu le trim ou pas      ");
	trim(test);
	std::cout << GREEN << test  << RESET << std::endl;

	/* print body */
	while (getline(ss, line))
		_body += "\n" + line;
	std::cout << CYAN << "Body: " << _body << RESET << std::endl;

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

t_rqType	Requests::getType(void) const
{
	return (_type);
}

std::string	Requests::getBody(void) const
{
	return (_body);
}

std::vector<std::pair<std::string,std::string> >	Requests::getQuery(void) const
{
	return (_query);
}
