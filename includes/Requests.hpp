/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Requests.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/21 10:24:52 by cblonde           #+#    #+#             */
/*   Updated: 2025/03/24 14:41:54 by cblonde          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTS_HPP
# define REQUESTS_HPP

# include <webserv.hpp>
# include <utils.hpp>
# include <Route.hpp>

class Client;

typedef enum	e_request_type
{
	GET,
	POST,
	DELETE,
	UNKNOW
} t_rqType;

class Requests
{
	private:
		std::map<std::string,std::string>	_headers;
		std::string							_requestUri;
		std::string							_documentUri;
		std::string							_pathInfo;
		std::string							_host;
		std::string							_path;
		std::string							_protocol;
		t_rqType							_type;
		std::string							_body;
		std::map<std::string,std::string>	_mimeTypes;
		std::string							_query;
		std::string							_fileName;
		int									_port;
		Route								*_conf;
		Client								&_client;
		/* handle in out */
		std::vector<unsigned char> 			_buffer;


		void	parse(std::string str);
		void	handlePath(void);
		void	handleHost(void);
	public:
		Requests(std::string str, Client  &client);
		Requests(Requests const &src);
		~Requests(void);
		Requests	&operator=(Requests const &rhs);
		void								checkConf(void);
		void								setConf(Route &conf);
		std::string							getProtocol(void) const;
		std::string							getPath(void) const;
		std::map<std::string,std::string> const	&getHeaders(void) const;
		std::string							getHost(void) const;
		std::string							getType(void) const;
		std::string const					&getBody(void) const;
		std::string							getQuery(void) const;
		std::string							getFileName(void) const;
		int									getPort(void) const;
		Route								&getConf(void) const;
		std::string							getRequestUri(void) const;
		std::string							getDocumentUri(void) const;
		std::string							getPathInfo(void) const;
		std::string							getContentType(void) const;
		std::string							getClientHostName(void) const;
		std::string							getClientIpStr(void) const;
		std::string							getClientPort(void) const;
};

#endif
