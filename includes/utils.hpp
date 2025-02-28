/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/24 10:26:29 by cblonde           #+#    #+#             */
/*   Updated: 2025/02/28 10:05:25 by cblonde          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

#include <webserv.hpp>

void						trim(std::string &str);
void						toUpper(std::string &str);
void						toLower(std::string &str);
std::vector<std::string>	split(std::string str, char sep);
std::pair<int,std::string>	getFile(std::string path);
void initMimeTypes(std::map<std::string, std::string> &mime_Types);
std::pair<int,std::string> openDir(std::string path);
void initResponseHeaders(std::map<std::string, std::string> &headers);
std::string	getFileType(std::string path);

#endif
