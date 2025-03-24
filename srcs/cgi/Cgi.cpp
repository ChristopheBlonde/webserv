/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.cpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <cblonde@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 07:14:01 by cblonde           #+#    #+#             */
/*   Updated: 2025/03/24 17:44:28 by cblonde          ###   ########.fr       */
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
			delete [] _env[i];
			_env[i] = NULL;
			i++;
		}
		delete [] _env;
		_env = NULL;
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
	Route	&reqConf = req.getConf();
	std::map<std::string, std::string>::const_iterator head;
	std::map<std::string, std::string>::iterator cgi;
	std::map<std::string, std::string> reqHeaders(req.getHeaders());
	std::string	tmp;

	for (head = reqHeaders.begin(); head != reqHeaders.end(); head++)
	{
		tmp = "HTTP_" + head->first;
		toUpper(tmp);
		_envMap[tmp] = head->second;
	}
	cgi = reqConf.getCgi().find(getFileType(req.getFileName()));
	_envMap["QUERY_STRING"] = req.getQuery();
	_envMap["REQUEST_METHOD"] = req.getType();
	_envMap["CONTENT_TYPE"] = req.getContentType();
	_envMap["CONTENT_LENGTH"] = to_string(req.getBody().size());
	_envMap["SCRIPT_NAME"] = req.getFileName();
	_envMap["PATH_INFO"] = req.getPathInfo();
	_envMap["REQUEST_URI"] = req.getRequestUri();
	_envMap["DOCUMENT_URI"] = req.getDocumentUri();
	_envMap["SERVER_PROTOCOL"] = "HTTP/1.1";
	_envMap["GATEWAY_INTERFACE"] = "CGI/1.0";
	_envMap["SERVER_SOFTWARE"] = "webserv/1.0";
	_envMap["REMOTE_ADDR"] = req.getClientHostName();
	_envMap["REMOTE_PORT"] = req.getClientPort();
	_envMap["SERVER_ADDR"] = "";
	_envMap["SERVER_PORT"] = "";
	_envMap["SERVER_NAME"] = "";
	_pid = -1;
	_contentSize = 0;
	_content = "";
	_status = -1;
	_scriptPath = reqConf.getRoot() + "/" + req.getFileName();
	_cgiPath = cgi->second;
	_env = NULL;
	std::cout << RED << "cgi path: " << _cgiPath << " script path: "
		<< _scriptPath << RESET << std::endl;
}

char	**Cgi::createEnvArr(void)
{
	size_t		i;
	std::string	tmp;
	_env = new char *[(_envMap.size() + 1 ) * sizeof(char *)];
	std::map<std::string, std::string>::iterator it;

	i = 0;
	memset(static_cast<void *>(_env), 0, (_envMap.size() + 1) * sizeof(char *));
	for (it = _envMap.begin(); it != _envMap.end(); it++)
	{
		tmp = it->first + "=" + it->second;
		_env[i] = new char[tmp.size() + 1];
		strcpy(_env[i], static_cast<const char *>(tmp.data()));
		i++;
	}	
	return (_env);
}

std::string	Cgi::execScript(void)
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
		return (_content);
	}
	FILE	*inFile = tmpfile();
	FILE	*outFile = tmpfile();
	int		fdIn = fileno(inFile);
	int		fdOut = fileno(outFile);
	char	buffer[1000];
	int		res = 1;

	write(fdIn, _body.data(), _body.size());
	fseek(inFile, 0, SEEK_SET);
	_pid = fork();
	if (_pid == -1)
	{
		_content = "Error: 500";
		return (_content);
	}
	else if (!_pid)
	{
		char const *argv[3] = {_cgiPath.data(), _scriptPath.data(), NULL};
		dup2(fdIn, 0);
		dup2(fdOut, 1);
		execve(argv[0], (char *const *)(argv), _env);
		std::cout << RED << "FAIL EXECVE: path: " << _scriptPath
			<< RESET << std::endl;
	}
	else
	{
		waitpid(-1, &_status, 0);
		fseek(outFile, 0, SEEK_SET);
		while (res != 0)
		{
			memset(buffer, 0, 1000);
			res = read(fdOut, buffer, 999);
			_content += buffer;
		}
	}
	//dup2(0, 0);
	//dup2(1, 1);
	fclose(inFile);
	fclose(outFile);
	close(fdIn);
	close(fdOut);
	if (!_pid)
		exit(0);
	std::cout << RED << "RESULT CGI :" << _content << RESET << std::endl;
	return _content;
}
