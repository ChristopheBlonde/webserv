/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 11:16:02 by cblonde           #+#    #+#             */
/*   Updated: 2025/04/10 06:57:13 by cblonde          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include "webserv.hpp"
# include "Requests.hpp"
# include "AutoIndex.hpp"
# include "ErrorPages.hpp"
# include "Cgi.hpp"
# include "Server.hpp"
# include <map>
# include <string>
# include <vector>
# include <sys/socket.h>
# include <poll.h>
# include <PollFd.hpp>
# include <cstdio>
# include <signal.h>

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
		std::string							_query;
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
		std::string							uploadPath;
		std::vector<std::string>			_cookies;
		pid_t								_pid;
		

		void	handleMethod(Requests const &req,
				std::map<std::string, std::string> const &headers);
		void	checkConnection(std::map<std::string,
				std::string> const &headers, std::string method);
		bool	checkMethod(std::string method);
		bool	checkContentLen();
		void	getStatFile(std::string path);
		void	uploadFile(std::map<std::string, std::string> const &headers);
		std::vector<FileData>	splitFiles(std::string boundary);
		void	openFiles(std::vector<FileData> vec);
		void	addFdToCluster(int fd, short event);
		bool	handleFileUpload(int fd);
		bool	handleFdCgi(int fd);
		bool	readPollFdFile(int fd);
		void	sendHeader(int fd);
		void	sendBody(int fd);
		void	deleteFile(void);
		void	getFileOrIndex();
		bool	checkExtCgi(void);
		void	getCgiHeader(bool &finded);
		void	handleCgiHeader(std::string &str);
		void	parseCookie(Requests req, std::map<std::string, std::string> const &cookies);
		void	createSession(void);
	public:
		Response(Requests const &req,Client &client, Server &server);
		~Response(void);

		void			createError(int stat);
		void			createResponseHeader(void);
		bool			handleInOut(struct pollfd &fd);
};

std::string	getResponseTypeStr(int stat);
std::string	getContentError(int stat);

#endif
