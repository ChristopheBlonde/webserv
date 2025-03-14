/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 11:16:02 by cblonde           #+#    #+#             */
/*   Updated: 2025/03/14 15:36:10 by cblonde          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <Requests.hpp>
# include <AutoIndex.hpp>

class Response
{
	private:
		int									_socket;
		std::string							_protocol;
		std::string							_body;
		std::string							_path;
		std::string							_host;
		int									_port;
		std::string							_fileName;
		std::string							_fileContent;
		struct pollfd						_fileStatus;
		std::map<std::string,std::string>	_headers;
		int									_contentLen;
		int									_status;
		bool								_autoIndex;
		bool								_cgi;
		std::string							_response;
		size_t								_resSize;
		std::map<std::string, std::string>	_mimeTypes;
		Response(void);
		void	isReferer(Requests const &req);
	public:
		Response(Requests const &req);
		Response(Response const &src);
		~Response(void);
		Response &operator=(Response const &rhs);

		std::string		getResponse(void) const;
		size_t			getResSize(void) const;
		struct pollfd	getFileStatus(void) const;
		int				getSocket(void) const;
		void			setResponse(std::string str);
		void			setFileStatus(struct pollfd &src);
		void			setFileContent(std::string &str);
		void			setSocket(int const socket);
		void	handleFile(void);
		void	createError(int stat);
		void	createResponse(void);
};

#endif
