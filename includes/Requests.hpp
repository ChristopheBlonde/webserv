/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Requests.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/21 10:24:52 by cblonde           #+#    #+#             */
/*   Updated: 2025/03/09 09:19:25 by cblonde          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTS_HPP
# define REQUESTS_HPP

# include <webserv.hpp>
# include <utils.hpp>

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
		Requests(void);
		std::map<std::string,std::string>					_headers;
		std::string											_host;
		std::string											_path;
		std::string											_protocol;
		t_rqType											_type;
		std::string											_body;
		std::vector<std::pair<std::string,std::string> >	_query;
		void	parse(std::string str);
	public:
		Requests(std::string str);
		Requests(Requests const &src);
		~Requests(void);
		Requests	&operator=(Requests const &rhs);

		std::string							getProtocol(void) const;
		std::string							getPath(void) const;
		std::map<std::string,std::string>	getHeaders(void) const;
		std::string							getHost(void) const;
		std::string							getType(void) const;
		std::string							getBody(void) const;
		std::vector<std::pair<std::string,std::string> >	getQuery(void) const;
};

#endif
