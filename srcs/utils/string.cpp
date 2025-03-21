/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 09:55:53 by cblonde           #+#    #+#             */
/*   Updated: 2025/03/21 13:39:46 by cblonde          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <utils.hpp>

void	formatHeader(std::string &str)
{
	std::string::iterator it;
	std::string::iterator ite;
	for (it = str.begin(); isspace(*it); it++);
	str.erase(str.begin(), it);
	for (ite = str.end(); isspace(*ite); ite--);
	str.erase(ite, str.end());
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
	for (it = str.begin(); it != str.end(); it++)
	{
		if (it == str.begin() || (it != str.begin() && *(it - 1) == '-'))
			*it = *it - 32;
	}
}

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

void	capitalize(std::string &str)
{
	int c = std::toupper(*(str.begin()));
	(void) c;
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

std::string	to_string(long int num)
{
	std::stringstream	tmp;

	tmp << num;
	return (tmp.str());
}
