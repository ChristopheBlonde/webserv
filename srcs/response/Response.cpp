/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 11:15:20 by cblonde           #+#    #+#             */
/*   Updated: 2025/03/29 08:56:05 by cblonde          ###   ########.fr       */
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
	this->_protocol = req.getProtocol();
	this->_host = req.getHost();
	this->_path = req.getPath();
	this->_port = req.getPort();
	this->_fileName = req.getFileName();
	this->_conf = &req.getConf();
	this->_cgi = _conf->getCgi().empty() ? false : true;
	this->_autoIndex = _conf->getAutoindex();
	this->_status = 200;
	initMimeTypes(_mimeTypes);
	initResponseHeaders(_headers);
	checkConnection(headers);
	if (!checkMethod(req.getType()))
		return ;
	if (!checkContentLen(headers))
		return ;
	isReferer(headers);
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
		/* upload */
	if ((req.getType() == "POST" || req.getType() == "DELETE")
			&& !_conf->getUploadDir().empty())
	{
		uploadFile(req.getHeaders());
	}
	else if (_cgi)
	{
		/* cgi */
		std::map<std::string, std::string> cgi(_conf->getCgi());
		for (std::map<std::string, std::string>::iterator it = cgi.begin();
				it != cgi.end(); it++)
		{
			if (!testAccess(it->second, EXIST) || !testAccess(it->second, EXECUTABLE))
			{
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
		/* no cgi */
		_fileFd = openDir(_path, _fileName, _conf->getIndex());
		getStatFile(_path + "/" + _fileName);
		if (_fileFd < 0)
		{
			createError(404);
			return ;
		}
		/* add fd to poll*/
		addFdToCluster(_fileFd, POLLIN);
	}
}

void	Response::createError(int stat)
{
	std::string	content;
	int			fd;

	if (!_autoIndex)
	{
		content = _server.getErrorPage(stat);
		if (!content.empty() && _status != 500)
		{
			std::cout << YELLOW << "custom error:" << content
				<< RESET << std::endl;
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
	}
	else
	{
		_status = 200;
		content = AutoIndex::generate(_path.data(), _host, _port);
	}
	_buffer.insert(_buffer.begin(), content.begin(), content.end());
	_status = stat;
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
	_headers["Content-Type"] += !_cgi && !_autoIndex
		? _mimeTypes[getFileType(_fileName)]
		: "text/html; charset=UFT-8";
	/* join all */
	for (it = _headers.begin(); it != _headers.end(); it++)
		_response += (it->second + "\r\n");
	_response += "\r\n";

	_headerReady = true;
	std::cout << CYAN << "Header ready" << RESET << std::endl;
	return ;
}

bool	Response::handleInOut(struct pollfd &fd)
{
	int				readByte;
	int				sentByte;
	unsigned char	buffer[FILE_BUFFER_SIZE];

//	std::cout << GREEN << "In handleInOut: fd: " << fd.fd
//		<< " envent: " << fd.events << " revent: " << fd.revents
//		<< RESET << std::endl;
	if (fd.fd == _cgiFd[0] && (fd.revents & POLLIN))
		return (handleFdCgi(fd.fd));
	if (fd.fd == _cgiFd[1] && (fd.revents & POLLOUT))
		return (handleFdCgi(fd.fd));
	if (fd.revents & POLLIN)
	{
		if (fd.fd == _fileFd)
		{
			readByte = read(fd.fd, buffer, FILE_BUFFER_SIZE - 1);
			if (readByte <= 0)
			{
				createResponseHeader();
				_fileFd = -1;
				return (false);
			}
			_buffer.insert(_buffer.end(), buffer, buffer + readByte);
		}
	}
	if (_buffer.empty() && fd.fd == -1)
		return (false);
	if (fd.revents & POLLOUT)
	{
		if (_filesUpload.find(fd.fd) != _filesUpload.end())
			return (handleFileUpload(fd.fd));
		if (!_headerSent && _headerReady)
		{
			if (!_response.empty())
			{
				sentByte = send(fd.fd, _response.c_str(),
						_response.size() < FILE_BUFFER_SIZE
						? _response.size() : FILE_BUFFER_SIZE, 0);
				_response.erase(0, sentByte);
			}
			else
				_headerSent = true;
		}
		if (!_buffer.empty() && _headerSent)
		{
			sentByte = send(fd.fd, _buffer.data(),
					_buffer.size() < FILE_BUFFER_SIZE
					? _buffer.size() : FILE_BUFFER_SIZE, 0);
			if (sentByte > 0)
				_buffer.erase(_buffer.begin(), _buffer.begin() + sentByte);
		}
		if (_buffer.empty() && _headerSent)
			return (false);
	}
	return (true);
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

std::string	Response::getResponse(void) const
{
	return (_response);
}

size_t	Response::getResSize(void) const
{
	return (_resSize);
}

int	Response::getFileFd(void) const
{
	return (_fileFd);
}

int	Response::getSocket(void) const
{
	return (_socket);
}

void	Response::setResponse(std::string str)
{
	_response = str;
}

void	Response::setSocket(int const socket)
{
	_socket = socket;
}
