/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Requests.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/21 10:46:49 by cblonde           #+#    #+#             */
/*   Updated: 2025/02/24 13:36:24 by cblonde          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Requests.hpp>

Requests::Requests(void)
{
	return ;
}

Requests::Requests(std::string str)
{
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

static void getMethod(std::string str, t_rqType &type)
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

static void getQuery(std::string &str,
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

static void	getHeaders(std::string str,
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
	getMethod(word, _type);
	getQuery(_path, _query);

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
		getHeaders(line, _headers);
	}

	/* Print headers*/
	std::map<std::string,std::string>::iterator ite;
	std::cout << GREEN << "headers contains:" << std::endl;
	for (ite=_headers.begin(); ite!=_headers.end(); ++ite)
		std::cout << GREEN << ite->first << " => " << ite->second << RESET << std::endl;

	/* test trim function */
	std::string test("   tu le trim ou pas      ");
	trim(test);
	std::cout << GREEN << test  << RESET << std::endl;

	/* print body */
	while (getline(ss, line))
		std::cout << CYAN << "Body: " << line << std::endl;

	return ;
}
