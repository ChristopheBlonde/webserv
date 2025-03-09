/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <cblonde@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 07:05:16 by cblonde           #+#    #+#             */
/*   Updated: 2025/03/09 09:33:46 by cblonde          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
# define CGI_HPP

# include <webserv.hpp>
# include <Requests.hpp>

class Cgi
{
	private:
		char								**_env;
		std::map<std::string,std::string>	_envMap;
		pid_t								_pid;
		int									_pipe[2];
		std::string							_content;
		int									_contentSize;
		std::string							_fileType;
		int									_status;
		std::string							_body;
		Cgi(void);
		void	execScript(void);
		char	**createEnvArr(void);
		void	initCgi(Requests const &req);
	public:
		Cgi(Requests const &req);
		Cgi(Cgi const &src);
		~Cgi(void);
		Cgi	&operator=(Cgi const &rhs);
};

#endif
