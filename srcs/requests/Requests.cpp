/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Requests.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/21 10:46:49 by cblonde           #+#    #+#             */
/*   Updated: 2025/02/21 17:50:48 by cblonde          ###   ########.fr       */
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

	index = str.find("?");
	if (index == std::string::npos)
		return ;
	if (str[index + 1] == '\0')
		return ;
	qstr = str.substr(index + 1);
	str = str.substr(0, index);
	std::stringstream test(qstr);
	while (getline(test, pair.first, '&'))
	{
		std::pair<std::string, std::string> pair;
		index = pair.first.find("=");
	//	if ()
		std::cout << RED << "pair: " << pair.first << RESET << std::endl;
	}
	std::vector<std::pair<std::string,std::string> >::iterator it;
	std::cout << "size: " << query.size() << std::endl;
	for (it = query.begin(); it != query.end(); it++)
	{
		std::cout << RED << "key: " << it->first << CYAN << " value: "
			<< it->second << std::endl << RESET;
	}
	return ;
}

void	Requests::parse(std::string str)
{
	std::stringstream	ss(str);
	std::string word;
	std::string method;
	std::string	line;
	getline(ss, line);
	std::cout << RED << line << std::endl;
	std::stringstream ssLine(line);

	ssLine >> method >> _path >> word;
	getMethod(method, _type);
	getQuery(_path, _query);
	return ;
}
