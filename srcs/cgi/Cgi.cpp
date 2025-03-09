/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <cblonde@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 07:14:01 by cblonde           #+#    #+#             */
/*   Updated: 2025/03/09 11:04:02 by cblonde          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Cgi.hpp>

Cgi::Cgi(void)
{
	return ;
}

Cgi::Cgi(Requests const &req) : _body(req.getBody())
{
	initCgi(req);
	return ;
}

Cgi::Cgi(Cgi const &src)
{
	*this = src;
	return ;
}

Cgi::~Cgi(void)
{
	size_t	i;

	i = 0;
	if (_env)
	{
		while (_env[i])
		{
			delete _env[i];
			i++;
		}
		delete [] _env;
	}
	return ;
}

Cgi	&Cgi::operator=(Cgi const &rhs)
{
	if (this != &rhs)
	{
	}
	return (*this);
}

void	Cgi::initCgi(Requests const &req)
{
	//root                /users/folder;
	//gzip                off;
	//fastcgi_pass        unix:/var/run/fcgiwrap.socket;
	_envMap["QUERY_STRING"] = "";
	_envMap["REQUEST_METHOD"] = req.getType();
	_envMap["CONTENT_TYPE"] = "";
	_envMap["CONTENT_LENGTH"] = "";
	_envMap["SCRIPT_NAME"] = "";
	_envMap["PATH_INFO"] = "";
	_envMap["REQUEST_URI"] = "";
	_envMap["DOCUMENT_URI"] = "";
	_envMap["SERVER_PROTOCOL"] = "HTTP/1.1";
	_envMap["GATEWAY_INTERFACE"] = "CGI/1.0";
	_envMap["SERVER_SOFTWARE"] = "webserv/1.0";
	_envMap["REMOTE_ADDR"] = "";
	_envMap["REMOTE_PORT"] = "";
	_envMap["SERVER_ADDR"] = "";
	_envMap["SERVER_PORT"] = "";
	_envMap["SERVER_NAME"] = "";
	_pid = -1;
	_pipe[0] = -1;
	_pipe[1] = -1;
	_contentSize = 0;
	_status = -1;
}

char	**Cgi::createEnvArr(void)
{
	size_t		i;
	std::string	tmp;
	char **env = new char *[_envMap.size() + 1];
	std::map<std::string, std::string>::iterator it;

	i = 0;
	memset(static_cast<void *>(env), 0, _envMap.size() + 1);
	for (it = _envMap.begin(); it != _envMap.end(); it++)
	{
		tmp = it->first + "=" + it->second;
		env[i] = new char[tmp.size() + 1];
		strcpy(env[i], static_cast<const char *>(tmp.data()));
		i++;
	}	
	return (env);
}

void	Cgi::execScript(void)
{
	try
	{
		_env = createEnvArr();
		size_t i = 0;
		while (_env[i])
		{
			std::cout << GREEN << "_envArr: [" << i << "] :" << _env[i]
				<< std::endl << RESET;
			i++;
		}
	}
	catch (std::bad_alloc &e)
	{
		std::cerr << RED << "Error: CGI: " << e.what() << std::endl << RESET;
		return ;
	}
	return ;
}
