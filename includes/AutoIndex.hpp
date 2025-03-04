/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AutoIndex.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <cblonde@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 07:39:04 by cblonde           #+#    #+#             */
/*   Updated: 2025/03/04 09:49:12 by cblonde          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef AUTOINDEX_HPP
# define AUTOINDEX_HPP

# include <webserv.hpp>

class AutoIndex
{
	private:
		char const	*_dirPath;
		std::string	_host;
		int			_port;

		AutoIndex(void);
		AutoIndex(char const *dirPath, std::string const &host, int port);
		AutoIndex(AutoIndex const &src);
		~AutoIndex(void);
		AutoIndex &operator=(AutoIndex const &rhs);
		std::string	getLink(std::string path, std::string d_name);
	public:
		static std::string generate(char const *dirPath, std::string const &host,
				int port);
};

#endif
