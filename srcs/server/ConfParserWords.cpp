/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfParserWords.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 18:27:52 by glaguyon          #+#    #+#             */
/*   Updated: 2025/03/10 19:16:08 by glaguyon         ###   ########.fr       */
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
	if (wordCount["listen"] > 1)
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
	try
	{
		server->addName(s);
	}
	catch (std::exception &e)
	{
		throw IncorrectArgException(std::string("dupliate server name: ") + e.what());
	}
	++argc;
	argv.push_back(s);
	good = true;
}
