/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfParserWords.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 18:27:52 by glaguyon          #+#    #+#             */
/*   Updated: 2025/04/01 23:01:22 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfParser.hpp"
#include "Cluster.hpp"
#include "Server.hpp"
#include "Route.hpp"
#include "Conf.hpp"

void	ConfParser::parseWordEmpty(const std::string &s)
{
	updateFunc(s);
}

void	ConfParser::parseWordServer(const std::string &s)
{
	if (!s[0])
		return;
	throw TooManyArgumentsException();
}

void	ConfParser::parseWordLocation(const std::string &s)
{
	if (!s[0])
		return;
	if (argc > 0)
		throw TooManyArgumentsException();
	if (!routes.empty()
		&& routes.top()->getName() != s.substr(0, routes.top()->getName().size()))
		throw IncorrectArgException("location is outside of base location");
	++argc;
	argv.push_back(s);
}

void	ConfParser::parseWordListen(const std::string &s)
{
	if (!routes.empty() || !server)
		throw KeywordWrongLevelException();
	if (wordCountServer["listen"] > 1)
		throw DuplicateKeywordException("duplicate listen found");
	if (!s[0])
		return;
	if (argc > 0)
		throw TooManyArgumentsException();

	char	*end;
	long		num = std::strtol(s.c_str(), &end, 10);
	uint8_t		ip[4] = {0};
	int		i = 0;

	while ((i < 3 && *end == '.') || i == 3)
	{
		if (errno == ERANGE || num < 0 || num > std::numeric_limits<uint8_t>::max())
			throw IncorrectArgException("incorrect host ip");
		ip[i++] = num;
		if (i < 4)
			num = std::strtol(++end, &end, 10);
	}
	if (i == 0)
	{
		if (errno == ERANGE || num < 1 || num > std::numeric_limits<uint16_t>::max()
			|| *end)
			throw IncorrectArgException("incorrect host port");
		server->setPort(htons(num));
	}
	else if (i == 4)
	{
		server->setIp(*reinterpret_cast<uint32_t *>(ip));
		if (*end == ':')
		{
			num = std::strtol(++end, &end, 10);
			if (errno == ERANGE || num < 1 || num > std::numeric_limits<uint16_t>::max()
				|| *end)
				throw IncorrectArgException("incorrect host port");
			server->setPort(htons(num));
		}
		else if (*end)
			throw IncorrectArgException("incorrect host ip");
	}
	else
		throw IncorrectArgException("incorrect host ip");

	++argc;
	argv.push_back(s);
	good = true;
}

void	ConfParser::parseWordServerName(const std::string &s)
{
	if (!routes.empty() || !server)
		throw KeywordWrongLevelException();
	if (!s[0])
		return;
	if (server->getNames().find(s) != server->getNames().end())
		throw IncorrectArgException("duplicate server name: " + s);
	server->addName(s);
	++argc;
	argv.push_back(s);
	good = true;
}

static void	checkCode(const std::string &s)
{
	char	*end;
	long	code = std::strtol(s.c_str(), &end, 10);
	
	if (*end)
		throw std::runtime_error("incorrect value");
	if (code < 300 || code > 599)
		throw std::runtime_error("value must be between 300 and 599");
}

void	ConfParser::parseWordErrorPage(const std::string &s)
{
	if (!routes.empty() || !server)
		throw KeywordWrongLevelException();
	if (s[0])
	{
		try
		{
			if (good)
				checkCode(argv[argc - 1]);
			checkCode(s);
		}
		catch (std::exception &e)
		{
			if (argc == 0 || good)
				throw IncorrectArgException(e.what());
			good = true;
		}
		++argc;
		argv.push_back(s);
	}
	if (end)
	{
		if (argc < 2)
			throw MissingArgsException();
		for (size_t i = 0; i < argc - 1; ++i)
			server->setErrorPage(
				std::strtol(argv[i].c_str(), NULL, 10), argv[argc - 1]);
		good = true;
	}
}

void	ConfParser::parseWordClientMaxBodySize(const std::string &s)
{
	if (!routes.empty() || !server)
		throw KeywordWrongLevelException();
	if (wordCountServer["client_max_body_size"] > 1)
		throw DuplicateKeywordException("duplicate client body size found");
	if (!s[0])
		return;
	if (argc > 0)
		throw TooManyArgumentsException();
	
	char	*end;
	long	num = std::strtol(s.c_str(), &end, 10);
	long	mul = 1;

	switch (*end)
	{
		case '\0':
			break;
		case 'k':
			mul = 1e3;
			break;
		case 'm':
			mul = 1e6;
			break;
		case 'g':
			mul = 1e9;
			break;
		default:
			throw IncorrectArgException("incorrect client body size value");
	}
	if (end[0] && end[1])
		throw IncorrectArgException("incorrect client body size value");
	if (errno == ERANGE || std::numeric_limits<long>::max() / mul < num)
		throw IncorrectArgException("client body size too large");
	server->setMaxSize(static_cast<size_t>(num * mul));
	++argc;
	argv.push_back(s);
	good = true;
}

void	ConfParser::parseWordAlias(const std::string &s)
{
	if (routes.empty())
		throw KeywordWrongLevelException();
	if (getWordCountLocation("root") + getWordCountLocation("alias") > 1)
		throw DuplicateKeywordException("duplicate root or alias found");
	if (!s[0])
		return;
	if (argc > 0)
		throw TooManyArgumentsException();
	routes.top()->setAlias(routes.top()->getName(), s);
	++argc;
	argv.push_back(s);
	good = true;
}

void	ConfParser::parseWordRoot(const std::string &s)
{
	if (routes.empty() && !server)
		throw KeywordWrongLevelException();
	if (wordCountServer["root"] > 1
		|| getWordCountLocation("root") + getWordCountLocation("alias") > 1)
		throw DuplicateKeywordException("duplicate root or alias found");
	if (!s[0])
		return;
	if (argc > 0)
		throw TooManyArgumentsException();
	if (!routes.empty())
		routes.top()->setRoot(s);
	else
		server->setRoot(s);
	++argc;
	argv.push_back(s);
	good = true;
}

void	ConfParser::parseWordMethods(const std::string &s)
{
	if (routes.empty() && !server)
		throw KeywordWrongLevelException();
	if (!s[0])
		return;
	if (std::find(Cluster::allowedMethods,
			Cluster::allowedMethods + Cluster::allowedMethodsSize, s)
		== Cluster::allowedMethods + Cluster::allowedMethodsSize)
		throw IncorrectArgException("unknown method");
	if (!routes.empty())
	{
		if (routes.top()->getAcceptedMethods().find(s)
			!= routes.top()->getAcceptedMethods().end())
			throw IncorrectArgException("duplicate method: " + s);
		routes.top()->addMethod(s);
	}
	else
	{
		if (server->getAcceptedMethods().find(s)
			!= server->getAcceptedMethods().end())
			throw IncorrectArgException("duplicate method: " + s);
		server->addMethod(s);
	}
	++argc;
	argv.push_back(s);
	good = true;
}

void	ConfParser::parseWordIndex(const std::string &s)
{
	if (routes.empty() && !server)
		throw KeywordWrongLevelException();
	if (!s[0])
		return;
	if (!routes.empty())
		routes.top()->addIndex(s);
	else
		server->addIndex(s);
	++argc;
	argv.push_back(s);
	good = true;
}

void	ConfParser::parseWordAutoindex(const std::string &s)
{
	bool	autoindex;

	if (routes.empty() && !server)
		throw KeywordWrongLevelException();
	if (wordCountServer["autoindex"] > 1 || getWordCountLocation("autoindex") > 1)
		throw DuplicateKeywordException("duplicate autoindex found");
	if (!s[0])
		return;
	if (argc > 0)
		throw TooManyArgumentsException();
	if (s == "on")
		autoindex = true;
	else if (s == "off")
		autoindex = false;
	else
		throw IncorrectArgException("autoindex is on or off");
	if (!routes.empty())
		routes.top()->setAutoindex(autoindex);
	else
		server->setAutoindex(autoindex);
	++argc;
	argv.push_back(s);
	good = true;
}

void	ConfParser::parseWordCgi(const std::string &s)
{
	if (!server && routes.empty())
		throw KeywordWrongLevelException();
	if (!s[0])
		return;
	if (argc > 1)
		throw TooManyArgumentsException();
	++argc;
	argv.push_back(s);
	if (argc < 2)
		return;
	good = true;
	if (!routes.empty())
	{
		if (routes.top()->getCgi()[argv[0]][0])
			throw IncorrectArgException("duplicate cgi extension found");
		routes.top()->addCgi(argv[0], argv[1]);
	}
	else
	{
		if (server->getCgi()[argv[0]][0])
			throw IncorrectArgException("duplicate cgi extension found");
		server->addCgi(argv[0], argv[1]);
	}
}

void	ConfParser::parseWordReturn(const std::string &s)
{
	if (routes.empty() && !server)
		throw KeywordWrongLevelException();
	if (wordCountServer["return"] > 1 || getWordCountLocation("return") > 1)
		throw DuplicateKeywordException("duplicate return found");
	if (!s[0])
		return;
	if (argc > 0)
		throw TooManyArgumentsException();
	if ((s.substr(0, 7) != "http://" && s.substr(0, 8) != "https://"))
		throw IncorrectArgException("return url must start with http:// or https://");
	if (!routes.empty())
		routes.top()->setRedirection(s);
	else
		server->setRedirection(s);
	++argc;
	argv.push_back(s);
	good = true;
}

void	ConfParser::parseWordUpload(const std::string &s)
{
	if (routes.empty() && !server)
		throw KeywordWrongLevelException();
	if (wordCountServer["upload"] > 1 || getWordCountLocation("upload") > 1)
		throw DuplicateKeywordException("duplicate upload directory found");
	if (!s[0])
		return;
	if (argc > 0)
		throw TooManyArgumentsException();
	if (!routes.empty())
		routes.top()->setUploadDir(s);
	else
		server->setUploadDir(s);
	++argc;
	argv.push_back(s);
	good = true;
}
