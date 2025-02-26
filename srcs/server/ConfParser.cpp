/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon           <skibidi@ohio.sus>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1833/02/30 06:67:85 by glaguyon          #+#    #+#             */
/*   Updated: 2025/02/26 23:10:52 by glaguyon         ###   ########.fr       */
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
EXC_FUNC(ConfParser, TooManyArgumentsException, "too many arguments provided");
EXC_FUNC(ConfParser, UnrecognizedKeywordException, "unrecognized keyword");

//static variables
const std::string		ConfParser::spaces = " \t\n";
const char			ConfParser::openBlock = '{';
const char			ConfParser::closeBlock = '}';
const char			ConfParser::comment = '#';

ConfParser::ConfParser(Cluster &cluster, const std::string &filename) : 
	file(filename.c_str()), 
	cluster(cluster), 
	server(NULL),
	routes(),
	currFunc(NULL),
	currWord(""),
	argc(0),
	line(0),
	i(0)
{
	if (!file)
		throw NoFileException();
	wordFunc[""] = &ConfParser::parseWordEmpty;
	wordFunc["server"] = &ConfParser::parseWordServer;
}

void	ConfParser::parseConf()
{
	std::string	s;

	std::getline(file, s);
	while (file.good())
	{
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
			{
				(this->*currFunc)(currWord);
				parseOpenBlock();
			}
			else if (s[i] == ConfParser::closeBlock)
			{
				(this->*currFunc)(currWord);
				parseCloseBlock();
			}
			else if (ConfParser::spaces.find(s[i]) != std::string::npos)
				(this->*currFunc)(currWord);
			else
				currWord += s[i];
			++i;
		}
		std::getline(file, s);
		++line;
	}
	if (server != NULL || !routes.empty())
		throw MissingCloseException();
}

void	ConfParser::updateFunc(const std::string &s)
{
	try
	{
		currFunc = wordFunc[currWord];
	}
	catch (std::exception &e)
	{
		throw UnrecognizedKeywordException();
	}
}

void	ConfParser::parseOpenBlock()
{
	if (currFunc == &ConfParser::parseWordServer)
	{
		if (server)
			//probleme server dans server
		server = cluster.addServer();
		currFunc = parseWordEmpty;
		currWord = "";
	}
	else if (currFunc == &ConfParser::parseWordLocation)
	{
		//push back dans les route de la route si y a
		//sinon route = route
	}
	else
		throw UnrecognizedKeywordException();
}

void	ConfParser::parseCloseBlock()
{
	if (currFunc != &ConfParser::parseWordEmpty)
		throw UnrecognizedKeywordException();
	if (!routes.empty())
		routes.pop();
	else if (server)
		server = NULL;
	else
		throw UnexpectedCloseException();
}

void	ConfParser::parseWordEmpty(std::string)
{
	updateFunc(currWord);
	currWord = "";
	argc = 0;
}

void	ConfParser::parseWordServer(std::string s)
{
	if (s[0])
		throw TooManyArgumentsException();
}

//openblock check que la fonction soit la bonne
//location si argc > 1 error c'est openblock qui doit l'enlever

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
