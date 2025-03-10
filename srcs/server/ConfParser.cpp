/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon           <skibidi@ohio.sus>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1833/02/30 06:67:85 by glaguyon          #+#    #+#             */
/*   Updated: 2025/03/10 22:13:36 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "ConfParser.hpp"
#include "Cluster.hpp"
#include "Server.hpp"
#include "Route.hpp"
#include "Conf.hpp"

//exceptions
EXC_FUNC(ConfParser, NoFileException, "can't open config file");
EXC_FUNC(ConfParser, UnexpectedCloseException, "unexpected closing character");
EXC_FUNC(ConfParser, UnexpectedOpenException, "unexpected opening character");
EXC_FUNC(ConfParser, MissingCloseException, "missing closing character");
EXC_FUNC(ConfParser, MissingEOLException, "missing EOL character");
EXC_FUNC(ConfParser, UnexpectedEOLException, "unexpected EOL character");
EXC_FUNC(ConfParser, TooManyArgumentsException, "too many arguments provided");
EXC_FUNC(ConfParser, UnrecognizedKeywordException, "unrecognized keyword");
EXC_FUNC(ConfParser, ServerInServerException, "can't have a server in a server");
EXC_FUNC(ConfParser, MissingServerException, "missing server block");
EXC_FUNC(ConfParser, MissingArgsException, "missing arguments");
EXC_FUNC(ConfParser, IncorrectArgException, "incorrect argument");
EXC_FUNC(ConfParser, KeywordWrongLevelException, "keyword is at wrong level");
EXC_FUNC(ConfParser, DuplicateKeywordException, "duplicate keyword found");

//static variables
const std::string		ConfParser::spaces = " \t\n";
const char			ConfParser::openBlock = '{';
const char			ConfParser::closeBlock = '}';
const char			ConfParser::comment = '#';
const char			ConfParser::endLine = ';';

ConfParser::ConfParser(Cluster &cluster, const std::string &filename) : 
	file(filename.c_str()), 
	cluster(cluster), 
	server(NULL),
	routes(),
	currFunc(&ConfParser::parseWordEmpty),
	currWord(""),
	argv(),
	argc(0),
	good(true),
	end(false),
	line(0),
	i(0)
{
	if (!file)
		throw NoFileException();
	wordFunc[""] = &ConfParser::parseWordEmpty;
	wordFunc["server"] = &ConfParser::parseWordServer;
	wordFunc["location"] = &ConfParser::parseWordLocation;
	wordFunc["listen"] = &ConfParser::parseWordListen;
	wordFunc["server_name"] = &ConfParser::parseWordServerName;
	wordFunc["error_page"] = &ConfParser::parseWordErrorPage;
	wordFunc["client_max_body_size"] = &ConfParser::parseWordClientMaxBodySize;
}

void	ConfParser::parseConf()
{
	std::string	s;

	std::getline(file, s);
	while (file.good())
	{
		s += '\n';
		if (s[0] == ConfParser::comment)
		{
			std::getline(file, s);
			++line;
			continue;
		}

		i = 0;
		while (s[i])
		{
			if (s[i] == ConfParser::openBlock)
				parseOpenBlock();
			else if (s[i] == ConfParser::closeBlock)
				parseCloseBlock();
			else if (s[i] == ConfParser::endLine)
				parseEndLine();
			else if (ConfParser::spaces.find(s[i]) != std::string::npos)
			{
				(this->*currFunc)(currWord);
				currWord = "";
			}
			else
				currWord += s[i];
			++i;
		}
		std::getline(file, s);
		++line;
	}
	if (server != NULL || !routes.empty())
		throw MissingCloseException();
	if (currFunc != &ConfParser::parseWordEmpty || currWord[0])
		throw MissingEOLException();
}

void	ConfParser::resetState()
{
	currWord = "";
	argc = 0;
	argv = std::vector<std::string>();
	good = true;
	end = false;
	currFunc = &ConfParser::parseWordEmpty;
}

void	ConfParser::parseEndLine()
{
	end = true;
	(this->*currFunc)(currWord);
	if (currFunc != &ConfParser::parseWordEmpty && !good)
		throw UnexpectedEOLException();
	resetState();
}

void	ConfParser::parseOpenBlock()
{
	(this->*currFunc)(currWord);
	if (currFunc == &ConfParser::parseWordServer)
	{
		if (server)
			throw ServerInServerException();
		server = cluster.addServer();
		resetState();
	}
	else if (currFunc == &ConfParser::parseWordLocation)
	{
		if (argc < 1)
			throw MissingArgsException();
		if (!server)
			throw MissingServerException();
		if (!routes.empty())
			routes.push(routes.top()->addRoute(argv[0]));
		else
			routes.push(server->addRoute(argv[0]));
		resetState();
	}
	else
		throw UnexpectedOpenException();
}

void	ConfParser::parseCloseBlock()
{
	(this->*currFunc)(currWord);
	if (currFunc != &ConfParser::parseWordEmpty)
		throw UnexpectedCloseException();
	if (!routes.empty())
		routes.pop();
	else if (server)
		server = NULL;
	else
		throw UnexpectedCloseException();
}

void	ConfParser::updateFunc(const std::string s)
{
	resetState();
	good = false;
	currFunc = wordFunc[s];
	if (currFunc == NULL)
		throw UnrecognizedKeywordException();
	if (currFunc == &ConfParser::parseWordLocation
		|| currFunc == &ConfParser::parseWordServer)
		wordCount.clear();
	wordCount[s] += 1;
}

void	ConfParser::fillMissingParams()
{
}

size_t	ConfParser::getLine()
{
	return line;
}

size_t	ConfParser::getI()
{
	return i;
}

ConfParser::~ConfParser()
{
}
