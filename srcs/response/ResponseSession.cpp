/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseSession.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 12:32:56 by cblonde           #+#    #+#             */
/*   Updated: 2025/04/10 06:58:06 by cblonde          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

void	Response::createSession(void)
{
	time_t		ttime;
	struct tm	*tmTime;
	char		buffer[1024];
	char		*bufferName = "abcdefijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	std::pair<std::string, std::string> sessionId;

	ttime = time(NULL);
	tmTime = gmtime(&ttime);
	strftime(buffer, 1024, "%a, %d %b %Y %T GMT", tmTime);
	sessionId.second = "{Date:{created:" + std::string(buffer)
		+ ", updated:"+ std::string(buffer) + "}}";
	for (int i = 0; i < 31; i++)
		buffer[i] = bufferName[rand() % std::strlen(bufferName)];
	buffer[31] = '\0';
	sessionId.first = std::string(buffer);
}

void	Response::parseCookie(Requests req, std::map<std::string, std::string> const &cookies)
{
	std::map<std::string, std::string>::const_iterator	itCook;
	std::pair<std::string, std::string>					tmp;
	std::vector<std::string>							arr;
	std::vector<std::string>::iterator					itArr;
	size_t												index;
	std::string											cookie;

	if (cookies.empty())
		return (createSession());
	itCook = cookies.find("Cookie");
	if (itCook == cookies.end())
		return (createSession());
	arr = split(itCook->second, ';');
	for (itArr = arr.begin(); itArr != arr.end(); itArr++)
	{
		index = itArr->find("=");
		if (index == std::string::npos)
			continue ;
		tmp.first = itArr->substr(0, index);
		formatHeader(tmp.first);
		tmp.second = itArr->substr(index + 1);
		trim(tmp.second);
		if (tmp.first == "Sessionid")
			return ;
	}
	return (createSession());
}
