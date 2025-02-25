/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfParser.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon           <skibidi@ohio.sus>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1833/02/30 06:67:85 by glaguyon          #+#    #+#             */
/*   Updated: 2025/02/25 19:40:21 by glaguyon         ###   ########.fr       */
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

//static variables
const std::string		ConfParser::spaces = " \t\n";
const char			ConfParser::openBlock = '{';
const char			ConfParser::closeBlock = '}';
const char			ConfParser::comment = '#';

ConfParser::ConfParser(Cluster &cluster, const std::string &filename) : cluster(cluster)
{
	std::ifstream	file(filename.c_str());
	
	if (!file)
		throw NoFileException();
	parseConfigFile(file);
}

void	ConfParser::parseWordServer()
{
	
}

ConfParser::~ConfParser()
{
}
	//instancier confighelp ?
void	ConfParser::parseConfigFile(std::ifstream &file)
{
	std::string	s;

	std::getline(file, s);
	while (file.good())
	{
		size_t	i = 0;

		while (s[i])
		{
			//si pas {} append au word ?
			while (spaces.find(s[i], 0) != std::string::npos)//?
				++i;
			if (s[i] == closeBlock && level-- == 0)
				throw UnexpectedCloseException();
			if (s[i] == openBlock)
			{
				if (!acceptOpen)
					throw UnexpectedOpenException();
				++level;
				acceptOpen = false;
			}
			else
				parseWord();//remplit le mot et les whitespaces valident ?
		}
		std::getline(file, s);
	}
	if (level)
		throw MissingCloseException();
}
