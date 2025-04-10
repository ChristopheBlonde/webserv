/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseSession.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 12:32:56 by cblonde           #+#    #+#             */
/*   Updated: 2025/04/10 15:24:32 by cblonde          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"

static std::string	getTimeNow(void)
{
	time_t		ttime;
	struct tm	*tmTime;
	char		buffer[1024];

	ttime = time(NULL);
	tmTime = gmtime(&ttime);
	strftime(buffer, 1024, "%a, %d %b %Y %T GMT", tmTime);
	return (static_cast<std::string>(buffer));
}

void	Response::updateSession(Requests const &req, std::string id)
{
	std::cout << CYAN << "Update session id: " << id << RESET << std::endl;
	size_t	start;
	size_t	end;
	std::string	cookie;

	cookie = req.searchSession(id);
	std::cout << "Cookie:" << cookie << std::endl;
	if (cookie.empty())
		return (createSession(req));
	start = cookie.find("updated:");
	if (start == std::string::npos)
		return ;
	start += 8;
	end = cookie.find("}", start);
	if (end == std::string::npos)
		return ;
	std::cout << GREEN << "before: " << cookie << RESET << std::endl;
	cookie.replace(cookie.begin() + start, cookie.begin() + end, getTimeNow());
	std::cout << GREEN << "after: " << cookie << RESET << std::endl;
}

void	Response::createSession(Requests const &req)
{
	char		buffer[1024];
	std::string		bufferName = "abcdefijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";
	std::pair<std::string, std::string> sessionId;

	sessionId.second = "{Date:{created:" + getTimeNow()
		+ ", updated:"+ getTimeNow() + "}}";
	for (int i = 0; i < 31; i++)
		buffer[i] = bufferName.c_str()[rand() % bufferName.size() - 1];
	buffer[31] = '\0';
	sessionId.first = std::string(buffer);
	_cookies.push_back("Sessionid=" + sessionId.first);
	req.addSession(sessionId);
}

void	Response::parseCookie(Requests const &req)
{
	std::map<std::string, std::string>::const_iterator	itHeaders;
	std::pair<std::string, std::string>					tmp;
	std::vector<std::string>							arr;
	std::vector<std::string>::iterator					itArr;
	size_t												index;
	std::string											cookie;

	if (req.getHeaders().empty())
		return (createSession(req));
	itHeaders = req.getHeaders().find("Cookie");
	if (itHeaders == req.getHeaders().end())
		return (createSession(req));
	arr = split(itHeaders->second, ';');
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
			return (updateSession(req, tmp.second));
	}
	return (createSession(req));
}
