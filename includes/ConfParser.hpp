/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfParser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon           <skibidi@ohio.sus>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1833/02/30 06:67:85 by glaguyon          #+#    #+#             */
/*   Updated: 2025/02/25 19:16:35 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFPARSER_HPP
# define CONFPARSER_HPP

#include <map>
#include <string>
#include <iostream>
#include <string>
#include "ExceptionTemplate.hpp"

class Cluster;
class Server;
class Conf;
class Route;

# define DEFAULT_CONF "./conf/def.conf"

class ConfParser
{
	//exceptions
	EXC_CLASS(NoFileException);
	EXC_CLASS(UnexpectedCloseException);
	EXC_CLASS(UnexpectedOpenException);
	EXC_CLASS(MissingCloseException);

	//static variables
	static const std::string		spaces;
	static const char			openBlock;
	static const char			closeBlock;
	static const char			comment;

	//stuff
	Cluster		&cluster;
	Server		*server;
	Route		*route;

	size_t		level;
	bool		acceptOpen;//faut un truc pour les locations
	std::map<std::string, void (ConfParser::*)(std::string)>	wordFunc;
	void (ConfParser::*currFunc)(std::string);
	std::string		currWord;
	void	parseConfigFile(std::ifstream &file);
	//ref vers server ou route
	size_t	argc;

	//mettre fonction prete a utiliser si arg
	//donner en arg ensuite les bons mots
	//faire un expecting ?

	public:
	ConfParser(Cluster &cluster, const std::string &filename);
	~ConfParser();
};
//le mot est donne a la fonction si y en a une, elle peut modifer currfunc
	//sinon la fonction appropriee est cherchee

#endif // CONFPARSER_HPP
