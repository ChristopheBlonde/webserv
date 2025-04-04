/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.cpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 11:15:20 by cblonde           #+#    #+#             */
/*   Updated: 2025/04/04 17:55:54 by glaguyon         ###   ########.fr       */
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
	//this->_port = req.getPort();
	this->_fileName = req.getFileName();
	//std::cout << RED << "Autoindex: " << _autoIndex << RESET << std::endl;
	initMimeTypes(_mimeTypes);
	initResponseHeaders(_headers);
	this->_status = req.getError();
	std::cout << YELLOW << "Status in request: " << _status << RESET << std::endl;
	if (_status != 200)//400, 404, 301, 308
	{
		if (req.getError() == 301 || req.getError() == 308)
		{
			_headers["Location"] = "Location: " + urlEncode(_path);//add query
			getStatFile("");
		}
		createResponseHeader();
		return;
	}
	this->_conf = &req.getConf();//maybe higher but uses conf so bad
	this->_cgi = _conf->getCgi().empty() ? false : true;
	this->_autoIndex = _conf->getAutoindex();
	checkConnection(headers);//TODO if error = 400 DO NOT USE CONF
	if (_status == 301 || _status == 302)//308 ?
	{
		std::string	redir = _conf->getRedirection();
		_headers["Location"] = "Location: "
			+ redir.substr(0, redir.find("://") + 3)//return query too
			+ urlEncode(redir.substr(redir.find("://") + 3));//WIP
		//FIXME encode only uri and query
		getStatFile("");
		createResponseHeader();
		return ;
	}
	//_path = handleBadPath(_conf->getRoot() + "/" + _path);
	std::cout << "path: " << _path << "\n";
	std::cout << "???1\n";
	if (!checkMethod(req.getType()))
		return ;
	std::cout << "???2\n";
	if (!checkContentLen(headers))
		return ;
	std::cout << "???3\n";


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
		this->_headers = rhs._headers;
		this->_autoIndex = rhs._autoIndex;
		this->_response = rhs._response;
		this->_resSize = rhs._resSize;
	}
	return (*this);
}

bool	Response::checkExtCgi(void)
{
	std::map<std::string, std::string> cgi(_conf->getCgi());
	std::map<std::string, std::string>::iterator it;
	std::string ext(getFileType(_fileName));

	if (_fileName.empty())
		return (false);
	for (it = cgi.begin(); it != cgi.end(); it++)
		if (it->first == ext)
			return (true);
	return (false);
}

void	Response::handleFile(Requests const &req)
{
	if (req.getType() == "DELETE")
		deleteFile();
	else if (req.getType() == "POST")
	{
		//&& !_conf->getUploadDir().empty() + check multipart
		//check exact match
		//rejeter ici si pas multipart
		std::cout << GREEN << "/* UPLOAD */" << RESET << std::endl;
		if (req.getType() == "POST")
			uploadFile(req.getHeaders());
		else if (_cgi && checkExtCgi())
		{
			std::cout << GREEN << "/* CGI */" << RESET << std::endl;
			std::map<std::string, std::string> cgi(_conf->getCgi());
			for (std::map<std::string, std::string>::iterator it = cgi.begin();
					it != cgi.end(); it++)
			{
				if (!testAccess(it->second, EXIST)
						|| !testAccess(it->second, EXECUTABLE))
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
	}
	else//get
	{
		std::cout << GREEN << "/* NORMAL */" << RESET << std::endl;
		_fileFd = openDir(_path, _fileName, _conf->getIndex());
		if (_fileFd < 0)
		{
			if (_fileName == "" && _autoIndex)
			{
				createError(200);
				return;
			}
			_status = 404;//hotfix
			std::cout << "404\n";
			std::cout << RED << "Error NORMAL fd" << RESET << std::endl;
			createError(404);
			return ;
		}
		getStatFile(_path + "/" + _fileName);
		addFdToCluster(_fileFd, POLLIN);
	}
}

//TODO must not use headers or conf if error = 400
void	Response::createError(int stat)
{
	std::string	content;
	int			fd;

	_status = stat;//idk
	std::cout << "je susi erreur\n";
	std::cout << "status " << _status << "\n";
	if (!_autoIndex || _status != 200)//XXX crappy hotfix
	//if (!_autoIndex || (_autoIndex && !_fileName.empty()))
	{
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
	{//TODO : pas de dir: 404, pas d'autoindex + dossier: 403
		//_status = 200;
		try
		{
		content = AutoIndex::generate(_conf->getMount(),
		_conf->getAliasedPart(), _path, _host);
		}
		catch (int code)//may work
		{
			std::cout << "fuck autoindex exploded\n";
			_status = code;
			createError(code);
			return ;
		}
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
	if (_status != 301 && _status != 302 && _status != 400)//added 400 hotfix
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
	std::cout << CYAN << "Header ready" << std::endl << _response
		<< RESET << std::endl;
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
	if (_fileName == "")
		return createError(403);
	
	std::string	path = _path + "/" + _fileName;

	if (!testAccess(path, EXIST))
		return createError(404);
	getStatFile(path);
	if (!std::remove(path.data()))
	{
		_status = 204;
		createResponseHeader();
	}
	else
		createError(500);
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
