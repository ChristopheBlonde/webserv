/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Conf.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon           <skibidi@ohio.sus>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1833/02/30 06:67:85 by glaguyon          #+#    #+#             */
/*   Updated: 2025/02/24 23:11:15 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Conf.hpp"

//exceptions
EXC_FUNC(Conf, ConfNoFileException, "can't open config file");
EXC_FUNC(Conf, ConfUnexpectedCloseException, "unexpected closing character");
EXC_FUNC(Conf, ConfUnexpectedOpenException, "unexpected opening character");
EXC_FUNC(Conf, ConfMissingCloseException, "missing closing character");

//static variables
const std::string		Conf::spaces = " \t\n";
const char			Conf::openBlock = '{';
const char			Conf::closeBlock = '}';

Conf::Conf(const std::string &filename)
{
	std::ifstream	file(filename.c_str());
	
	if (!file)
		throw ConfNoFileException();
	parseConfigFile(file);
	//instancier confighelp ?
}

void	Conf::parseConfigFile(std::ifstream &file)
{
	std::string	s;

	std::getline(file, s);
	while (file.good())
	{
		size_t	i = 0;

		while (s[i])
		{
			//si pas {} append au word ?
			while (Conf::spaces.find(s[i], 0) != std::string::npos)//?
				++i;
			if (s[i] == Conf::closeBlock && level-- == 0)
				throw ConfUnexpectedCloseException();
			if (s[i] == Conf::openBlock)
			{
				if (!acceptOpen)
					throw ConfUnexpectedOpenException();
				++level;
				acceptOpen = false;
			}
			else
				parseWord();//remplit le mot et les whitespaces valident ?
		}
		std::getline(file, s);
	}
	if (level)
		throw ConfMissingCloseException();
}

Conf::~Conf()
{
}
