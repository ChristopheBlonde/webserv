/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Response.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 11:16:02 by cblonde           #+#    #+#             */
/*   Updated: 2025/02/26 12:13:26 by cblonde          ###   ########.fr       */
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
		std::vector<std::string>	_headers;
		int							_contentLen;
		int							_status;
		bool						_autoIndex;
		Response(void);
	public:
		Response(Requests const &req);
		Response(Response const &src);
		~Response(void);
		Response &operator=(Response const &rhs);

		void	createError(int stat);
};

#endif
