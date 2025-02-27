/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 11:16:02 by cblonde           #+#    #+#             */
/*   Updated: 2025/02/27 14:23:05 by cblonde          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef RESPONSE_HPP
# define RESPONSE_HPP

# include <Requests.hpp>

class Response
{
	private:
		std::string					_protocol;
		std::string					_body;
		std::string					_path;
		std::string					_host;
		std::vector<std::string>	_headers;
		int							_contentLen;
		int							_status;
		bool						_autoIndex;
		std::string					_response;
		size_t						_resSize;
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
