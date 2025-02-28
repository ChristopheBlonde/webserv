/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfParser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon           <skibidi@ohio.sus>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1833/02/30 06:67:85 by glaguyon          #+#    #+#             */
/*   Updated: 2025/02/28 17:36:09 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFPARSER_HPP
# define CONFPARSER_HPP

#include <stack>
#include <map>
#include <vector>
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
	EXC_CLASS(MissingEOLException);
	EXC_CLASS(UnexpectedEOLException);
	EXC_CLASS(TooManyArgumentsException);
	EXC_CLASS(UnrecognizedKeywordException);
	EXC_CLASS(ServerInServerException);
	EXC_CLASS(MissingServerException);
	EXC_CLASS(MissingArgsException);

	//static variables
	static const std::string						spaces;
	static const char							openBlock;
	static const char							closeBlock;
	static const char							comment;
	static const char							endLine;

	//stuff
	std::ifstream								file;
	Cluster									&cluster;
	Server									*server;
	std::stack<Route *>							routes;
	std::map<std::string, void (ConfParser::*)(const std::string &)>	wordFunc;
	void	(ConfParser::*currFunc)(const std::string &);
	std::string								currWord;
	std::vector<std::string>						argv;
	size_t									argc;
	bool									good;
	size_t									line;
	size_t									i;

	void	resetState();
	void	parseEndLine();
	void	parseOpenBlock();
	void	parseCloseBlock();
	void	updateFunc(const std::string s);
	void	parseWordEmpty(const std::string &s);
	void	parseWordServer(const std::string &S);
	void	parseWordLocation(const std::string &S);

	public:
	ConfParser(Cluster &cluster, const std::string &filename);
	~ConfParser();
	void	parseConf();
	void	fillMissingParams();
	size_t	getLine();
	size_t	getI();
};

#endif // CONFPARSER_HPP
