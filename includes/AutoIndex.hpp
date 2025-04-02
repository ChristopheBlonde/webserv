/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   AutoIndex.hpp                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <cblonde@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/03/03 07:39:04 by cblonde           #+#    #+#             */
/*   Updated: 2025/04/02 19:10:04 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef AUTOINDEX_HPP
# define AUTOINDEX_HPP

# include <string>
# include <dirent.h>
# include "webserv.hpp"
# include "utils.hpp"

class AutoIndex
{
	private:
		std::string	_dirPath;
		std::string	_host;

		AutoIndex(const std::string &dirPath, const std::string &host);
		AutoIndex(AutoIndex const &src);
		~AutoIndex(void);
		AutoIndex &operator=(AutoIndex const &rhs);
		std::string	getLink(std::string path, std::string d_name);
		static std::string htmlEncode(std::string s, bool isAttribute);
	public:
		static std::string generate(const std::string &mount,
					const std::string &aliased,
					const std::string &dirPath,
					const std::string &host);
};

#endif
