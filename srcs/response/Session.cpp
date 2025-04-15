/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Session.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/09 12:32:56 by cblonde           #+#    #+#             */
/*   Updated: 2025/04/15 14:22:58 by cblonde          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Session.hpp"
#include "Response.hpp"

Session::Session(Requests const &req, Response &res)
	: _req(req),
	_res(res)
{
	parseCookie();
	return ;
}

Session::~Session(void)
{
	return ;
}

static std::string	getTimeNow(void)
{
	time_t		ttime;
	struct tm	*tmTime;
	char		buffer[1024];

	ttime = time(NULL);
	tmTime = gmtime(&ttime);
	if (!tmTime)
		return (std::string("Error: time conversion"));
	strftime(buffer, 1024, "%a, %d %b %Y %T GMT", tmTime);
	return (static_cast<std::string>(buffer));
}

static std::string	getRandomId(void)
{
	char		buffer[32];
	std::string	const	chars = "abcdefijklmnopqrstuvwxyz0123456789ABCDEFGHIJKLMNOPQRSTUVWXYZ";

	for (int i = 0; i < 31; i++)
		buffer[i] = chars[rand() % (chars.size() - 1)];
	buffer[31] = '\0';
	return (std::string(buffer));
}

void	Session::updateSession(std::string id)
{
	std::string	cookie;
	std::vector<std::pair<std::string, std::string> >::iterator it;

	cookie = _req.searchSession(id);
	if (cookie.empty())
		return (createSession());
	setSession(cookie);
	for (it = _session.begin(); it != _session.end(); it++)
	{
		if (it->first == "Updated")
		{
			//std::cout << YELLOW << "Date before: " << it->second << RESET << std::endl;
			it->second = getTimeNow();
			//std::cout << GREEN << "Date after: " << it->second << RESET << std::endl;
		}
	}
}

void	Session::createSession(void)
{
	std::pair<std::string, std::string> pair;

	pair.first = "Created";
	pair.second = getTimeNow();
	_session.push_back(pair);
	pair.first = "Updated";
	_session.push_back(pair);
	pair.first = "Sessionid";
	pair.second = getRandomId();
	_res.setCookie("Sessionid=" + pair.second);
	pair.first = pair.second;
	pair.second = getSession();
	_req.addSession(pair);
}

void	Session::parseCookie(void)
{
	std::map<std::string, std::string>::const_iterator	itHeaders;
	std::pair<std::string, std::string>					tmp;
	std::vector<std::string>							arr;
	std::vector<std::string>::iterator					itArr;
	size_t												index;
	std::string											cookie;

	if (_req.getHeaders().empty())
		return (createSession());
	itHeaders = _req.getHeaders().find("Cookie");
	if (itHeaders == _req.getHeaders().end())
		return (createSession());
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
			return (updateSession(tmp.second));
	}
	return (createSession());
}

std::string	Session::getSession(void) const
{
	std::string	tmp;
	std::vector<std::pair<std::string, std::string> >::const_iterator it;

	for (it = _session.begin(); it != _session.end(); it++)
		tmp += it->first + "=" + it->second + ";";
	return (tmp);
}

void	Session::setSession(std::string const &str)
{
	size_t	start;
	size_t	end;
	std::pair<std::string, std::string> pair;

	start = 0;
	while ((end = str.find_first_of("=", start))
			!= std::string::npos)
	{
		pair.first = str.substr(start, end - start);
		start = end + 1;
		end = str.find_first_of(";", start);
		if (end != std::string::npos)
		{
			pair.second = str.substr(start, end - start);
			start = end + 1;
		}
		else
		{
			pair.second = str.substr(start);
			start = std::string::npos;
		}
		_session.push_back(pair);
	}
	return ;
}
