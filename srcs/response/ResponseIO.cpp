/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ResponseIO.cpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon <marvin@42.fr>                    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/07 22:58:57 by glaguyon          #+#    #+#             */
/*   Updated: 2025/04/08 19:43:24 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "Response.hpp"
#include "Client.hpp"

bool	Response::handleInOut(struct pollfd &fd)
{
	if (fd.fd == _cgiFd[0] && (fd.revents & (POLLIN | POLLHUP)))
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
	{
		std::cout << RESET << "-----------\n";
		std::cout << YELLOW << std::string(_buffer.begin(), _buffer.begin() + sentBytes) << "\n";
		std::cout << RESET << "-----------\n";
		_buffer.erase(_buffer.begin(), _buffer.begin() + sentBytes);
	}
}

void	Response::addFdToCluster(int fd, short event)
{
	PollFd	pfd;

	pfd.fd = fd;
	pfd.events = event;
	pfd.revents = 0;

	_client.addResponseFd(pfd);
}

extern volatile sig_atomic_t sig;

bool	Response::handleFdCgi(int fd)
{
	int		readBytes;
	int		sentBytes;
	int		bodySize = _body.size();
	char	buffer[FILE_BUFFER_SIZE];
	static bool	head = false;

	if (fd == _cgiFd[0])
	{
		readBytes = read(fd, buffer, FILE_BUFFER_SIZE - 1);
		if (readBytes <= 0)
		{
			head = false;
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
		std::cout << RED << buffer << "\n";
		_buffer.insert(_buffer.end(), buffer, buffer + readBytes);
		std::cout << CYAN << std::string(_buffer.begin(), _buffer.end()) << "\n";
		std::cout << RESET << "===========================\n";
		if (!head)
			getCgiHeader(head);
	}
	else
	{
		sentBytes = write(fd, _body.data(), bodySize < FILE_BUFFER_SIZE
				? bodySize : FILE_BUFFER_SIZE);
		if (sentBytes <= 0 || sig == SIGPIPE)
		{
			if (sig == SIGPIPE)
				sig = 0;
			_cgiFd[1] = -1;
			close(fd);
			return (false);
		}
		_body.erase(0, sentBytes);
	}
	return (true);
}
