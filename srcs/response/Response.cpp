/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 11:15:20 by cblonde           #+#    #+#             */
/*   Updated: 2025/03/31 21:04:43 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <Response.hpp>
#include <Client.hpp>

Response::Response(Requests const &req, Client  &client, Server &server)
	: _client(client),
	_server(server)
{
	std::map<std::string, std::string> const &headers = req.getHeaders();

	this->_body = req.getBody();
	this->_headerSent = false;
	this->_headerReady = false;
	this->_sizeSend = 0;
	this->_fileFd = -1;
	this->_cgiFd[0] = -1;
	this->_cgiFd[1] = -1;
	this->_status = 200;
	this->_protocol = req.getProtocol();
	if (_protocol.empty())
	{
		this->_status = 400;
		_protocol = "HTTP/1.1";
		createResponseHeader();
		return;
	}
	std::cout << "AAAAAAAAAAAAAAAA" << _protocol << "\n";
	this->_host = req.getHost();
	this->_path = req.getPath();
	this->_port = req.getPort();
	this->_fileName = req.getFileName();
	this->_conf = &req.getConf();
	this->_cgi = _conf->getCgi().empty() ? false : true;
	this->_autoIndex = _conf->getAutoindex();
	initMimeTypes(_mimeTypes);
	initResponseHeaders(_headers);
	checkConnection(headers);
	if (_status == 301 || _status == 302)
	{
		std::cout << "i301\n";
		_headers["Location"] = "Location: " + _conf->getRedirection();
		getStatFile("");
		createResponseHeader();
		return ;
	}
	if (!checkMethod(req.getType()))
		return ;
	if (!checkContentLen(headers))
		return ;

	_path = _conf->getRoot() + "/" + _path;
	handleBadPath(_path);
	std::cout << "path: " << _path << "\n";

	//check for 400 or 301 (directory, redirection to path)

	handleFile(req);
	return ;
}

Response::Response(Response const &src)
	: _client(src._client),
	_server(src._server)
{
	*this = src;
	return ;
}

Response::~Response(void)
{
	return ;
}

Response	&Response::operator=(Response const &rhs)
{
	if (this != &rhs)
	{
		this->_protocol = rhs._protocol;
		this->_body = rhs._body;
		this->_path = rhs._path;
		this->_host = rhs._host;
		this->_port = rhs._port;
		this->_headers = rhs._headers;
		this->_autoIndex = rhs._autoIndex;
		this->_response = rhs._response;
		this->_resSize = rhs._resSize;
	}
	return (*this);
}

void	Response::handleFile(Requests const &req)
{
	if ((req.getType() == "POST" || req.getType() == "DELETE")
			&& !_conf->getUploadDir().empty())
	{
		if (req.getType() == "POST")
			uploadFile(req.getHeaders());
		else
			deleteFile();
	}
	else if (_cgi)
	{
		std::map<std::string, std::string> cgi(_conf->getCgi());
		for (std::map<std::string, std::string>::iterator it = cgi.begin();
				it != cgi.end(); it++)
		{
			if (!testAccess(it->second, EXIST) || !testAccess(it->second, EXECUTABLE))
			{
				std::cout << "a\n";
				createError(400);
				return ;
			}
		}
		Cgi cgiObj(req, _server);
		int status = cgiObj.execScript();
		if (status == 200)
		{
			_cgiFd[0] = cgiObj.getChildFd();
			_cgiFd[1] = cgiObj.getParentFd();
			addFdToCluster(cgiObj.getChildFd(), POLLIN);
			addFdToCluster(cgiObj.getParentFd(), POLLOUT);
		}
		else
			createError(status);
	}
	else
	{
		std::cout << "here\n";
		_fileFd = openDir(_path, _fileName, _conf->getIndex());
		if (_fileFd < 0)
		{
			createError(404);
			return ;
		}
		getStatFile(_path + "/" + _fileName);
		addFdToCluster(_fileFd, POLLIN);
	}
}

void	Response::createError(int stat)
{
	std::string	content;
	int			fd;

	std::cout << "je susi erreur\n";
	if (!_autoIndex)
	{
		std::cout << "non\n";
		content = _server.getErrorPage(stat);
		if (!content.empty() && _status != 500)
		{
			fd = getFile(content);
			if (fd == -1)
			{
				createError(500);
				return ;
			}
			addFdToCluster(fd, POLLIN);
			getStatFile(content);
			_fileFd = fd;
			_status = stat;
			return ;
		}
		content = ERROR_PAGE(getResponseTypeStr(stat), getContentError(stat),
				to_string(stat));
		getStatFile("");
		_headers["Content-Type"] += "text/html; charset=UFT-8";
		_headers.erase("Last-Modified");
		_status = stat;
	}
	else
	{
		std::cout << "oui\n";
		_status = 200;
		content = AutoIndex::generate(_path.data(), _host, _port);
	}
	_buffer.insert(_buffer.begin(), content.begin(), content.end());
	createResponseHeader();
	return ;
}

void	Response::createResponseHeader(void)
{
	std::map<std::string,std::string>::iterator it;

	_response = _protocol + " "
		+ to_string(_status) + " "
		+ getResponseTypeStr(_status)
		+ "\r\n";
	_headers["Content-Length"] += to_string(_buffer.size());
	if (_status != 301 && _status != 302)
	{
		std::cout << "salut: " << _mimeTypes[getFileType(_fileName)] << "\n";
		std::cout << getFileType(_fileName) << "\n";
		std::cout << "header: \"" << _headers["Content-Type"] << "\"\n";
		_headers["Content-Type"] += (!_cgi || !_autoIndex || (
							_autoIndex && _fileName.empty()))
			? _mimeTypes[getFileType(_fileName)]
			: "text/html; charset=UFT-8";
		std::cout << "header: \"" << _headers["Content-Type"] << "\"\n";
	}
	else
		_headers.erase("Content-Type");
	for (it = _headers.begin(); it != _headers.end(); it++)
		_response += (it->second + "\r\n");
	_response += "\r\n";

	_headerReady = true;
	std::cout << CYAN << "Header ready" << _response << RESET << std::endl;
	return ;
}

bool	Response::handleInOut(struct pollfd &fd)
{
	if (fd.fd == _cgiFd[0] && (fd.revents & POLLIN))
		return (handleFdCgi(fd.fd));
	if (fd.fd == _cgiFd[1] && (fd.revents & POLLOUT))
		return (handleFdCgi(fd.fd));
	if (fd.fd == _fileFd && fd.revents & POLLIN)
		return (readPollFdFile(fd.fd));
	if (_buffer.empty() && fd.fd == -1)
		return (false);
	if (fd.revents & POLLOUT
			&& _filesUpload.find(fd.fd) != _filesUpload.end())
		return (handleFileUpload(fd.fd));
	if (fd.revents & POLLOUT
			&& !_headerSent && _headerReady)
		sendHeader(fd.fd);
	if (fd.revents & POLLOUT
			&& !_buffer.empty() && _headerSent)
		sendBody(fd.fd);
	if (_buffer.empty() && _headerSent)
		return (false);
	return (true);
}

bool	Response::readPollFdFile(int fd)
{
	int		readBytes;
	char	buffer[FILE_BUFFER_SIZE];

	readBytes = read(fd, buffer, FILE_BUFFER_SIZE - 1);
	if (readBytes <= 0)
	{
		createResponseHeader();
		_fileFd = -1;
		return (false);
	}
	_buffer.insert(_buffer.end(), buffer, buffer + readBytes);
	return (true);
}

void	Response::sendHeader(int fd)
{
	int sentBytes;

	if (!_response.empty())
	{
		sentBytes = send(fd, _response.c_str(),
				_response.size() < FILE_BUFFER_SIZE
				? _response.size() : FILE_BUFFER_SIZE, 0);
		_response.erase(0, sentBytes);
	}
	else
			_headerSent = true;
}

void	Response::sendBody(int fd)
{
	int	sentBytes;
	sentBytes = send(fd, _buffer.data(),
			_buffer.size() < FILE_BUFFER_SIZE
			? _buffer.size() : FILE_BUFFER_SIZE, 0);
	if (sentBytes > 0)
		_buffer.erase(_buffer.begin(), _buffer.begin() + sentBytes);
}

void	Response::addFdToCluster(int fd, short event)
{
	PollFd	pfd;

	pfd.fd = fd;
	pfd.events = event;
	pfd.revents = 0;

	_client.addResponseFd(pfd);
}

bool	Response::handleFdCgi(int fd)
{
	int	readBytes;
	int	sentBytes;
	int	bodySize = _body.size();
	char	buffer[FILE_BUFFER_SIZE];

	if (fd == _cgiFd[0])
	{
		readBytes = read(fd, buffer, FILE_BUFFER_SIZE - 1);
		if (readBytes <= 0)
		{
			close(fd);
			if (_cgiFd[1] > 0)
			{
				close(_cgiFd[1]);
				_cgiFd[1] = -1;
			}
			_cgiFd[0] = -1;
			createResponseHeader();
			return (false);
		}
		buffer[readBytes] = '\0';
		_buffer.insert(_buffer.end(), buffer, buffer + readBytes);
	}
	else
	{
		sentBytes = write(fd, _body.data(), bodySize < FILE_BUFFER_SIZE
				? bodySize : FILE_BUFFER_SIZE);
		if (sentBytes <= 0)
		{
			_cgiFd[1] = -1;
			close(fd);
			return (false);
		}
		_body.erase(0, sentBytes);
	}
	return (true);
}

void	Response::deleteFile(void)
{
	std::string	path = _conf->getUploadDir();

	if (!testAccess(path, DIRACCESS)
			|| !testAccess(path + "/" + _fileName, EXIST))
		createError(404);
	else
	{
		path = path + "/" + _fileName;
		getStatFile(path);
		if (!std::remove(path.data()))
		{
			_status = 204;
			createResponseHeader();
		}
		else
			createError(500);
	}
}

int	Response::getFileFd(void) const
{
	return (_fileFd);
}

int	Response::getSocket(void) const
{
	return (_socket);
}

void	Response::setSocket(int const socket)
{
	_socket = socket;
}
