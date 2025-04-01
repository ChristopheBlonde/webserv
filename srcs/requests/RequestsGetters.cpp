/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   RequestsGetters.cpp                                :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/30 20:21:52 by glaguyon          #+#    #+#             */
/*   Updated: 2025/04/01 12:29:57 by cblonde          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Requests.hpp"
#include "Client.hpp"

std::string	Requests::getProtocol(void) const
{
	return (_protocol);
}

std::string	Requests::getPath(void) const
{
	return (_path);
}

std::map<std::string,std::string> const	&Requests::getHeaders(void) const
{
	return (_headers);
}

std::string	Requests::getHost(void) const
{
	return (_host);
}

std::string const	&Requests::getBody(void) const
{
	return (_body);
}

std::string	Requests::getQuery(void) const
{
	return (_query);
}

std::string Requests::getFileName(void) const
{
	return (_fileName);
}

int			Requests::getPort(void) const
{
	return (_port);
}

Route	&Requests::getConf(void) const
{
	return (*_conf);
}

std::string	Requests::getDocumentUri(void) const
{
	return (_documentUri);
}

std::string Requests::getRequestUri(void) const
{
	return (_requestUri);
}

std::string	Requests::getPathInfo(void) const
{
	return (_pathInfo);
}

std::string Requests::getClientHostName(void) const
{
	return (_client.getHostName());
}

std::string Requests::getClientIpStr(void) const
{
	return (_client.getIpStr());
}

std::string Requests::getClientPort(void) const
{
	return (_client.getPortStr());
}

std::string	Requests::getType(void) const
{
	std::string key[3] = {"GET", "POST", "DELETE"};
	t_rqType	types[3] = {GET, POST, DELETE};
	for (size_t i = 0; i < 3; i++)
	{
		if (_type == types[i])
			return (key[i]);
	}
	return ("");
}

std::string Requests::getContentType(void) const
{
	std::map<std::string, std::string>::const_iterator it;
	std::string	contentType = "";
	size_t	index;

	it = _headers.find("Content-Type");
	if (it != _headers.end())
	{
		index = it->second.find(";");
		if (index != std::string::npos)
			contentType = it->second.substr(0, index);
		else
			contentType = it->second;
	}
	return (contentType);
}

int	Requests::getError() const
{
	return error;
}
