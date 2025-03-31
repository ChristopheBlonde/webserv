/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 11:16:02 by cblonde           #+#    #+#             */
/*   Updated: 2025/03/31 15:31:48 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <webserv.hpp>
# include <Requests.hpp>
# include <AutoIndex.hpp>
# include <ErrorPages.hpp>
# include <Cgi.hpp>
# include <Server.hpp>
# include <map>
# include <string>
# include <vector>
# include <sys/socket.h>
# include <poll.h>
# include <PollFd.hpp>
# include <cstdio>

struct	FileData
{
	std::string	fileName;
	char const	*start;
	size_t		size;
	size_t		offset;
};

class Client;

class Response
{
	private:
		int									_status;
		Route								*_conf;
		Client								&_client;
		Server								&_server;
		int									_socket;
		std::string							_protocol;
		std::string							_body;
		std::string							_path;
		std::string							_host;
		int									_port;
		std::string							_fileName;
		std::map<std::string,std::string>	_headers;
		bool								_autoIndex;
		bool								_cgi;
		std::string							_response;
		int									_resSize;
		std::map<std::string, std::string>	_mimeTypes;
		std::map<int, FileData>				_filesUpload;
		std::vector<unsigned char>			_buffer;
		int									_fileFd;
		int									_cgiFd[2];
		bool								_headerSent;
		bool								_headerReady;
		int									_sizeSend;
		void	handleFile(Requests const &req);
		void	checkConnection(std::map<std::string,
				std::string> const &headers);
		bool	checkMethod(std::string method);
		bool	checkContentLen(std::map<std::string,
				std::string> const &headers);
		void	getStatFile(std::string path);
		void	uploadFile(std::map<std::string, std::string> const &headers);
		std::string	handleBoundary(std::string &boundary,
				size_t &step, size_t &currStart, std::string &filename);
		void	addFdToCluster(int fd, short event);
		bool	handleFileUpload(int fd);
		bool	handleFdCgi(int fd);
		bool	readPollFdFile(int fd);
		void	sendHeader(int fd);
		void	sendBody(int fd);
		void	deleteFile(void);
	public:
		Response(Requests const &req,Client &client, Server &server);
		Response(Response const &src);
		~Response(void);
		Response &operator=(Response const &rhs);

		int				getFileFd(void) const;
		int				getSocket(void) const;
		void			setSocket(int const socket);
		void			createError(int stat);
		void			createResponseHeader(void);
		bool			handleInOut(struct pollfd &fd);
};

std::string	getResponseTypeStr(int stat);
std::string	getContentError(int stat);

#endif
