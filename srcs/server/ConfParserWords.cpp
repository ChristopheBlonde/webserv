/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfParserWords.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/04 18:27:52 by glaguyon          #+#    #+#             */
/*   Updated: 2025/03/05 20:34:07 by glaguyon         ###   ########.fr       */
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
