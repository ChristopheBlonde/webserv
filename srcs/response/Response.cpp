/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 11:15:20 by cblonde           #+#    #+#             */
/*   Updated: 2025/04/09 16:50:57 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include "Client.hpp"

Response::Response(Requests const &req, Client  &client, Server &server)
	: _client(client),
	_server(server)
{
	std::map<std::string, std::string> const &headers = req.getHeaders();

	this->_socket = req.getFd();
	this->_body = req.getBody();
	this->_headerSent = false;
	this->_headerReady = false;
	this->_sizeSend = 0;
	this->_fileFd = -1;
	this->_cgiFd[0] = -1;
	this->_cgiFd[1] = -1;
	this->_protocol = req.getProtocol();
	this->_host = req.getHost();
	this->_path = req.getPath();
	this->_fileName = req.getFileName();
	this->_query = req.getQuery();
	this->_status = req.getError();
	std::cout << "status " << _status << "\n";
	this->_pid = -1;
	this->_conf = &req.getConf();
	this->uploadPath = "";
	this->_cgi = false;
	this->_autoIndex = false;
	if (_conf != NULL)
	{
		this->uploadPath = _conf->getUploadDir();
		if (uploadPath != "" && uploadPath[uploadPath.size() -1] != '/')
			uploadPath += "/";
		this->_cgi = !_conf->getCgi().empty() && checkExtCgi();
		this->_autoIndex = _conf->getAutoindex();
	}
	initMimeTypes(_mimeTypes);
	checkConnection(headers, req.getType());
	if (_status != 200)//400, 404, 301, 308, 501
	{
		if (_status / 100 * 100 == 300)
		{
			_headers["Location"] = urlEncode(_path);
			if (_query != "")
				_headers["Location"] += "?" + _query;
			getStatFile("");
			createResponseHeader();
			return;
		}
		createError(_status);
		return;
	}
	if (_status / 100 * 100 == 300)
	{
		std::string	redir = _conf->getRedirection();

		_headers["Location"] = redir.substr(0, redir.find("://") + 3)
			+ urlEncode(redir.substr(redir.find("://") + 3,
				redir.find("?") - redir.find("://") - 3));
		if (_query != "")
			_headers["Location"] += "?" + _query;
		getStatFile("");
		createResponseHeader();
		return ;
	}
	if (!checkMethod(req.getType()))
	{
		createError(405);
		return ;
	}
	if (!checkContentLen())
		return ;
	handleMethod(req, headers);
}

Response::~Response(void)
{
	if (_pid != -1)
	{
		waitpid(_pid, NULL, WNOHANG);
		kill(_pid, 9);
	}
	return ;
}

void	Response::handleMethod(Requests const &req,
	std::map<std::string, std::string> const &headers)
{
	std::string	method = req.getType();

	if (method == "POST" && _fileName == ""
		&& _conf->getAliasedPart() + _path.substr(_conf->getMount().size())
		== _conf->getName() + ((_conf->getName()[_conf->getName().size() - 1] == '/') ? "" : "/"))
	{
		if (headers.at("Content-Type").size() < 19
			|| headers.at("Content-Type").compare(0, 19, "multipart/form-data")
			|| std::string(" \t;\0", 4).find(headers.at("Content-Type")[19]) == std::string::npos)
			createError(415);
		else
			uploadFile(req.getHeaders());
	}
	else if (_cgi)
	{
		std::map<std::string, std::string> cgi(_conf->getCgi());
		for (std::map<std::string, std::string>::iterator it = cgi.begin();
				it != cgi.end(); it++)
		{
			if (_fileName.size() >= it->first.size()
				&& _fileName.compare(_fileName.size() - it->first.size(),
				it->first.size(), it->first) == 0)
			{
				if (!testAccess(it->second, EXIST)
						|| !testAccess(it->second, EXECUTABLE))
				{
					createError(403);
					return ;
				}
				break;
			}
		}
		Cgi cgiObj(req, _server);
		int status = cgiObj.execScript();
		
		if (status == 200)
		{
			_pid	= cgiObj.getPid();
			_cgiFd[0] = cgiObj.getChildFd();
			_cgiFd[1] = cgiObj.getParentFd();
			addFdToCluster(cgiObj.getChildFd(), (POLLIN | POLLHUP));
			addFdToCluster(cgiObj.getParentFd(), POLLOUT);
			_headers.erase("Last-Modified");
		}
		else
		{
			_cgi = false;
			createError(status);
		}
	}
	else if (method == "DELETE")
		deleteFile();
	else if (method == "POST")
	{
		if (headers.at("Content-Type").size() >= 19
			&& headers.at("Content-Type").compare(0, 19, "multipart/form-data") == 0
			&& std::string(" \t;\0", 4).find(headers.at("Content-Type")[19]) != std::string::npos)
			createError(403);
		else
			createError(400);
	}
	else if (method == "GET")
		getFileOrIndex();
}

void	Response::createError(int stat)
{
	std::string	content;
	int			fd;

	_status = stat;
	if (_autoIndex && _status == 200 && _fileName == "")
	{
		try
		{
			content = AutoIndex::generate(_conf->getMount(),
			_conf->getAliasedPart(), _path, _host);
			getStatFile("");
			_headers.erase("Last-Modified");
		}
		catch (int code)
		{
			_status = code;
			createError(code);
			return ;
		}
	}
	else
	{
		if (!_buffer.empty())
		{
			getStatFile("");
			return ;
		}
		content = _server.getErrorPage(stat);
		if (!content.empty())
		{
			fd = getFile(content);
			if (fd != -1)
			{
				addFdToCluster(fd, POLLIN);
				getStatFile(content);
				_fileFd = fd;
				return;
			}
		}
		content = ERROR_PAGE(getResponseTypeStr(stat), getContentError(stat),
				to_string(stat));
		getStatFile("");
		_headers["Content-Type"] = "text/html; charset=UFT-8";
		_headers.erase("Last-Modified");
	}
	_buffer.insert(_buffer.begin(), content.begin(), content.end());
	createResponseHeader();
	return ;
}

void	Response::createResponseHeader(void)
{
	std::map<std::string,std::string>::iterator it;
	std::vector<std::string>::iterator			itCookie;

	_response = _protocol + " "
		+ to_string(_status) + " "
		+ getResponseTypeStr(_status)
		+ "\r\n";
	_headers["Content-Length"] = to_string(_buffer.size());
	if (_status / 100 * 100 != 300 && _buffer.size() > 0)
	{
		if (!_cgi && _status / 100 * 100 == 200
				&& _fileName != "" && _mimeTypes[getFileType(_fileName)] != "")
			_headers["Content-Type"] = _mimeTypes[getFileType(_fileName)];
		else if (!_cgi)
		{
			if (_status / 100 * 100 != 200 || (_autoIndex && _fileName.empty()))
				_headers["Content-Type"] = "text/html; charset=UTF-8";
			else
				_headers["Content-Type"] = "text/plain; charset=UTF-8";
		}
	}
	else
		_headers.erase("Content-Type");
	for (it = _headers.begin(); it != _headers.end(); it++)
		_response += it->first + ": " + it->second + "\r\n";
	for (itCookie = _cookies.begin(); itCookie != _cookies.end(); itCookie++)
		_response += "Set-Cookie: " + *itCookie + "\r\n";
	_response += "\r\n";
	std::cout << CYAN << _response << RESET;
	_headerReady = true;
	return ;
}
