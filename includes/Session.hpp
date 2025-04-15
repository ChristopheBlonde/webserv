/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Session.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <cblonde@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/04/13 10:01:31 by cblonde           #+#    #+#             */
/*   Updated: 2025/04/15 12:13:12 by cblonde          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SESSION_HPP
# define SESSION_HPP

# include <vector>
# include "Requests.hpp"

class Response;

class Session
{
	private:
		std::vector<std::pair<std::string, std::string> >	_session;
		Requests	const &_req;
		Response	&_res;
		void	createSession(void);
		void	updateSession(std::string id);
		void	parseCookie(void);
	public:
		Session(Requests const &req, Response &res);
		~Session(void);
		std::string	getSession(void) const;
		void	setSession(std::string const &str);
};

#endif
