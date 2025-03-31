/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 09:55:53 by cblonde           #+#    #+#             */
/*   Updated: 2025/03/31 20:08:33 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"

std::string	&urlDecode(std::string &s)
{
	std::cout << "FIX ME\n";
	return s;
}

//Kebab-Case
std::string	&formatHeader(std::string &str)
{
	bool	dash = true;

	trim(str);
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
	for (std::string::iterator it = str.begin(); it != str.end(); ++it)
	{
		if (dash)
			*it = std::toupper(*it);
		dash = *it == '-';
	}
	return str;
}

std::string	&trim(std::string &str)
{
	size_t	start = str.find_first_not_of(" \t");
	size_t	end = str.find_last_not_of(" \t");

	if (start == std::string::npos)
		str = "";
	else
		str = str.substr(start, end - start + 1);
	return str;
}

std::string	&toUpper(std::string &str)
{
	std::transform(str.begin(), str.end(), str.begin(), ::toupper);
	return str;
}

std::string	&toLower(std::string &str)
{
	std::transform(str.begin(), str.end(), str.begin(), ::tolower);
	return str;
}

std::string	&capitalize(std::string &str)
{
	str[0] = std::toupper(str[0]);
	return str;
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

std::string	&handleBadPath(std::string &str)
{
	size_t			i = 0;

	while (i < str.size())
	{
		if (str[i] == '/')
		{
			//rm ./ and ../
			while ((str.size() - i >= 3 && str.compare(i + 1, 2, "./") == 0)
				|| (str.size() - i >= 4 && str.compare(i + 1, 3, "../") == 0))
				str.erase(i + 1, str.find_first_of('/', i + 1) - i);
			//rm duplicate /
			if (str[i + 1] == '/')
			{
				str.erase(i + 1, str.find_first_not_of('/', i + 1) - i - 1);
				continue;
			}
		}
		++i;
	}
	//rm last /. or /..
	if ((str.size() >= 2 && str.compare(str.size() - 2, 2, "/.") == 0)
		|| (str.size() >= 3 && str.compare(str.size() - 3, 3, "/..") == 0))
		str.erase(str.find_last_of("/") + 1, str.size() - str.find_last_of("/") - 1);
	return str;
}
