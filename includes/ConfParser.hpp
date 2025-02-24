/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   ConfParser.hpp                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon           <skibidi@ohio.sus>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1833/02/30 06:67:85 by glaguyon          #+#    #+#             */
/*   Updated: 2025/02/24 23:14:29 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef CONFPARSER_HPP
# define CONFPARSER_HPP

#include <map>
#include <string>
#include <iostream>

class ConfParser
{
	size_t		level;
	bool		acceptOpen;//faut un truc pour les locations
	std::map<std::string, >	wordFunc;//func en second
	std::string		currWord
	//ref vers server ou route

	//mettre fonction prete a utiliser si arg
	//donner en arg ensuite les bons mots
	//faire un expecting ?

	public:
	ConfParser();
	~ConfParser();
};

#endif // CONFPARSER_HPP
