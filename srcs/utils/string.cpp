/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 09:55:53 by cblonde           #+#    #+#             */
/*   Updated: 2025/03/24 14:29:24 by cblonde          ###   ########.fr       */
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

void	handleBadPath(std::string &str)
{
	std::string::iterator	it;
	size_t					i;

	std::cout << GREEN << "before: " << str << RESET << std::endl;
	i = str.find_last_of("/");
	if (i == str.size() - 1 && i != 0)
		str.erase(i);
	for (it = str.begin(); it != str.end(); it++)
	{
		if (*it == '/' && (*(it + 1) == '/' || *(it + 1) == '.'))
		{
			str.erase(it + 1);
			it--;
		}
	}
	std::cout << GREEN << "after: " << str << RESET << std::endl;
}
