/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Conf.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon           <skibidi@ohio.sus>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1833/02/30 06:67:85 by glaguyon          #+#    #+#             */
/*   Updated: 1833/02/30 06:67:85 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "Conf.hpp"

Conf::Conf()
{
	std::cout << "hello\n";
}

Conf::Conf(const Conf &c)
{
	*this = c;
	std::cout << "hello hello\n";
}

Conf &Conf::operator=(const Conf &c)
{
	std::cout << "hello = hello\n";
	return *this;
}

Conf::~Conf()
{
	std::cout << "bye bye\n";
}
