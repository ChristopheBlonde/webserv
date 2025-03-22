/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/24 10:26:29 by cblonde           #+#    #+#             */
/*   Updated: 2025/03/22 13:54:17 by cblonde          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

#include <webserv.hpp>

void						trim(std::string &str);
void						toUpper(std::string &str);
void						toLower(std::string &str);
void						capitalize(std::string &str);
void						formatHeader(std::string &str);
std::vector<std::string>	split(std::string str, char sep);
int							getFile(std::string path);
void						initMimeTypes(std::map<std::string,
		std::string> &mime_Types);
int							openDir(std::string path, std::string &fileName,
		std::vector<std::string> &files);
void						initResponseHeaders(std::map<std::string,
		std::string> &headers);
std::string					getFileType(std::string path);
std::string					to_string(long int num);

#endif
