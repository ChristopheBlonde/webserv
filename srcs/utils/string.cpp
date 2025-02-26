/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 09:55:53 by cblonde           #+#    #+#             */
/*   Updated: 2025/02/26 10:42:15 by cblonde          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <utils.hpp>

void	trim(std::string &str)
{
	std::string::iterator it;
	std::string::iterator ite;
	for (it = str.begin(); isspace(*it); it++);
	str.erase(str.begin(), it);
	for (ite = str.end(); isspace(*ite); ite--);
	str.erase(ite, str.end());
	return ;
}

void	toUpper(std::string &str)
{
	std::transform(str.begin(), str.end(), str.begin(), ::toupper);
	return ;
}

void	toLower(std::string &str)
{
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
	return ;
}

std::vector<std::string>	split(std::string str, char sep)
{
	std::vector<std::string>	elements;
	std::string					line;
	std::stringstream			ss(str);

	while (std::getline(ss, line, sep))
		elements.push_back(line);
	return (elements);
}
