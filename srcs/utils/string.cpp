/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   string.cpp                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 09:55:53 by cblonde           #+#    #+#             */
/*   Updated: 2025/04/07 22:54:49 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "utils.hpp"

static inline unsigned char hexCharToValue(unsigned char c)
{
	if (c >= '0' && c <= '9')
		return c - '0';
	if (c >= 'A' && c <= 'F')
		return 10 + (c - 'A');
	if (c >= 'a' && c <= 'f')
		return 10 + (c - 'a');
	return 0;
}

static std::string	urlDecode(std::string encoded, bool query)
{
	std::string	decoded;

	for (size_t i = 0; i < encoded.length(); ++i)
	{
		if (encoded[i] == '+' && query)
			decoded += ' ';
		else if (encoded[i] == '%' && i + 2 < encoded.length()
			&& (query || 
			(encoded.compare(i, 3, "%2F") && encoded.compare(i, 3, "%2F"))))
		{
			const unsigned char h = encoded[i + 1];
			const unsigned char l = encoded[i + 2];

			if (std::isxdigit(h) && std::isxdigit(l))
			{
				const unsigned char byte = 
					(hexCharToValue(h) << 4) | hexCharToValue(l);
				decoded += byte;
				i += 2;
			}
			else
				decoded += encoded[i];
		}
		else
			decoded += encoded[i];
	}
	return decoded;
}

static std::string	urlEncode(std::string decoded, bool query)
{
	static const char	hex[] = "0123456789ABCDEF";
	std::string		encoded;

	for (size_t i = 0; i < decoded.length(); ++i)
	{
		const unsigned char	c = decoded[i];
		
		if (isalnum(c) || c == '-' || c == '_' || c == '.' || c == '~'
			|| (c == '/' && !query))
			encoded += c;
		else if (c == ' ' && query)
			encoded += '+';
		else
		{
			encoded += '%';
			encoded += hex[c >> 4];
			encoded += hex[c & 0xF];
		}
	}
	return encoded;
}

std::string	urlEncode(std::string s)
{
	return urlEncode(s, 0);
}

std::string	urlDecode(std::string s)
{
	return urlDecode(s, 0);
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

std::string	to_string(long int num)
{
	std::stringstream	tmp;

	tmp << num;
	return (tmp.str());
}

std::string	handleBadPath(std::string str)
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
