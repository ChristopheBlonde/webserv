/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/24 10:26:29 by cblonde           #+#    #+#             */
/*   Updated: 2025/04/07 22:22:19 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

# include <webserv.hpp>
# include <cerrno>
# include <cstring>
# include <algorithm>
# include <ctime>
# include <string>
# include <sstream>
# include <iostream>
# include <vector>
# include <map>
# include <sys/stat.h>
# include <unistd.h>
# include <dirent.h>
# include <sys/fcntl.h>

typedef enum e_access
{
	EXIST,
	READABLE,
	WRITEABLE,
	EXECUTABLE,
	DIRACCESS
}	t_access;

std::string						handleBadPath(std::string str);
std::string						urlEncode(std::string s);
std::string						urlEncodeQuery(std::string s);
std::string						urlDecode(std::string s);
std::string						urlDecodeQuery(std::string s);
std::string						&trim(std::string &str);
std::string						&toUpper(std::string &str);
std::string						&toLower(std::string &str);
std::string						&capitalize(std::string &str);
std::string						&formatHeader(std::string &str);
//std::vector<std::string>	split(std::string str, char sep);
int							getFile(std::string path);
int							openFileUpload(std::string path);
void						initMimeTypes(std::map<std::string,
		std::string> &mime_Types);
void						initResponseHeaders(std::map<std::string,
		std::string> &headers);
std::string					getFileType(std::string path);
std::string					to_string(long int num);
bool						testAccess(std::string path, t_access test);

#endif
