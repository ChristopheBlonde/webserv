/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Server.hpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/20 11:46:41 by cblonde           #+#    #+#             */
/*   Updated: 2025/03/18 13:30:09 by cblonde          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef SERVER_HPP
# define SERVER_HPP

#include <webserv.hpp>
#include <Requests.hpp>
#include <Response.hpp>
#include <utils.hpp>

#define BUFFER_SIZE 1024

class Server {
	private:
		std::vector<pollfd>	_fds;
		long int		_socket;
		short int		_port;
		std::string		_address;	
		std::map<int, std::string>	requests;
		//std::map<int, std::string>	files;
		std::map<int, Response *>	ress;
		std::map<int, Response *> files;
		Server(void);
		void	handleRequests(struct pollfd &fd);
		void	handleFiles(void);
	public:
		Server(short int port);
		Server(Server const &src);
		~Server(void);
		Server &operator=(Server const &rhs);
		long int	init(void);
		void		run(void);
		void		get_client_maybe(void);
		class ServerException : public std::exception
		{
			private:
				std::string	_str;
			public:
				ServerException(std::string const str);
				virtual ~ServerException(void) throw();
				virtual const char *what(void) const throw();
				const char	*getStr(void) const;
		};
};

#endif
