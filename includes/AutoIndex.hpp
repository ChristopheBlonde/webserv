/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AutoIndex.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <cblonde@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 07:39:04 by cblonde           #+#    #+#             */
/*   Updated: 2025/04/02 15:56:54 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef AUTOINDEX_HPP
# define AUTOINDEX_HPP

# include <string>
# include <sstream>
# include <dirent.h>
# include "webserv.hpp"
# include "utils.hpp"

class AutoIndex
{
	private:
		char const	*_dirPath;
		std::string	_host;

		AutoIndex(void);
		AutoIndex(char const *dirPath, std::string const &host);
		AutoIndex(AutoIndex const &src);
		~AutoIndex(void);
		AutoIndex &operator=(AutoIndex const &rhs);
		std::string	getLink(std::string path, std::string d_name);
	public:
		static std::string generate(std::string s1, std::string s2, char const *dirPath, std::string const &host);
};

#endif
