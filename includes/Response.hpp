/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 11:16:02 by cblonde           #+#    #+#             */
/*   Updated: 2025/03/10 14:41:27 by cblonde          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <Requests.hpp>
# include <AutoIndex.hpp>

class Response
{
	private:
		std::string							_protocol;
		std::string							_body;
		std::string							_path;
		std::string							_host;
		int									_port;
		std::map<std::string,std::string>	_headers;
		int									_contentLen;
		int									_status;
		bool								_autoIndex;
		bool								_cgi;
		std::string							_response;
		size_t								_resSize;
		std::map<std::string, std::string> _mimeTypes;
		Response(void);
	public:
		Response(Requests const &req);
		Response(Response const &src);
		~Response(void);
		Response &operator=(Response const &rhs);

		std::string	getResponse(void);
		size_t		getResSize(void);
		void	createError(int stat);
		void	createResponse(void);
};

#endif
