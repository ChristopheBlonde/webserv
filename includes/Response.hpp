/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 11:16:02 by cblonde           #+#    #+#             */
/*   Updated: 2025/03/27 17:41:50 by cblonde          ###   ########.fr       */
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
		bool								_headerSent;
		bool								_headerReady;
		int									_sizeSend;
		void	handleFile(Requests const &req);
		void	isReferer(std::map<std::string, std::string> const &headers);
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
	public:
		Response(Requests const &req,Client &client, Server &server);
		Response(Response const &src);
		~Response(void);
		Response &operator=(Response const &rhs);

		std::string		getResponse(void) const;
		size_t			getResSize(void) const;
		int				getFileFd(void) const;
		int				getSocket(void) const;
		void			setResponse(std::string str);
		void			setSocket(int const socket);
		void			createError(int stat);
		void			createResponseHeader(void);
		bool			handleInOut(struct pollfd &fd);
};

#endif
