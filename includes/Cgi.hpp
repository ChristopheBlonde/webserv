/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cgi.hpp                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <cblonde@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/06 07:05:16 by cblonde           #+#    #+#             */
/*   Updated: 2025/04/09 16:22:50 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CGI_HPP
# define CGI_HPP

# include <cstdio>
# include <sys/wait.h>
# include "Server.hpp"
# include "webserv.hpp"
# include "Requests.hpp"
# include "Route.hpp"
# include "utils.hpp"

class Cgi
{
	private:
		char								**_env;
		std::map<std::string,std::string>	_envMap;
		pid_t								_pid;
		std::string							_fileType;
		int									_status;
		std::string							_scriptPath;
		std::string							_cgiPath;
		int									_parentToChild[2];
		int									_childToParent[2];
		Cgi(void);
		char	**createEnvArr(void);
		void	initCgi(Requests const &req, Server &server);
	public:
		Cgi(Requests const &req, Server &server);
		Cgi(Cgi const &src);
		~Cgi(void);
		Cgi	&operator=(Cgi const &rhs);
		int	execScript(void);
		int	&getChildFd(void);
		int	&getParentFd(void);
		pid_t	getPid();
		void	closePipes();
};

#endif
