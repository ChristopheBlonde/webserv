/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfParser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon           <skibidi@ohio.sus>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1833/02/30 06:67:85 by glaguyon          #+#    #+#             */
/*   Updated: 2025/02/26 23:00:11 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFPARSER_HPP
# define CONFPARSER_HPP

#include <stack>
#include <map>
#include <string>
#include <iostream>
#include <fstream>
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
	EXC_CLASS(TooManyArgumentsException);
	EXC_CLASS(UnrecognizedKeywordException);

	//static variables
	static const std::string						spaces;
	static const char							openBlock;
	static const char							closeBlock;
	static const char							comment;

	//stuff
	std::ifstream								file;
	Cluster									&cluster;
	Server									*server;
	std::stack<Route *>							routes;
	std::map<std::string, void (ConfParser::*)(const std::string &)>	wordFunc;
	void	(ConfParser::*currFunc)(const std::string &);
	std::string								currWord;
	size_t									argc;
	size_t									line;
	size_t									i;

	void	parseOpenBlock();
	void	parseCloseBlock();
	void	updateFunc(const std::string &s);
	void	parseWordEmpty(const std::string &s);
	void	parseWordServer(const std::string &S);

	public:
	ConfParser(Cluster &cluster, const std::string &filename);
	~ConfParser();
	void	parseConf();
	void	fillMissingParams();
	size_t	getLine();
	size_t	getI();
};

#endif // CONFPARSER_HPP
