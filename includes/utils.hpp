/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.hpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/24 10:26:29 by cblonde           #+#    #+#             */
/*   Updated: 2025/02/26 10:42:49 by cblonde          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef UTILS_HPP
# define UTILS_HPP

#include <webserv.hpp>

void						trim(std::string &str);
void						toUpper(std::string &str);
void						toLower(std::string &str);
std::vector<std::string>	split(std::string str, char sep);

#endif
