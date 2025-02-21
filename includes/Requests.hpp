/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Requests.hpp                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/21 10:24:52 by cblonde           #+#    #+#             */
/*   Updated: 2025/02/21 14:51:25 by cblonde          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef REQUESTS_HPP
# define REQUESTS_HPP

# include <webserv.hpp>

typedef enum	e_request_type
{
	GET,
	POST,
	DELETE
} t_rqType;

class Requests
{
	private:
		Requests(void);
		std::map<std::string,std::string>					_headers;
		std::string											_host;
		std::string											_path;
		t_rqType											_type;
		std::vector<std::pair<std::string,std::string> >	_query;
		void	parse(std::string str);
	public:
		Requests(std::string str);
		Requests(Requests const &src);
		~Requests(void);
		Requests	&operator=(Requests const &rhs);
};

#endif
