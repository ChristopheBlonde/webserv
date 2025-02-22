/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Route.cpp                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon           <skibidi@ohio.sus>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1833/02/30 06:67:85 by glaguyon          #+#    #+#             */
/*   Updated: 1833/02/30 06:67:85 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "Route.hpp"

Route::Route()
{
	std::cout << "hello\n";
}

Route::Route(const Route &r)
{
	*this = r;
	std::cout << "hello hello\n";
}

Route &Route::operator=(const Route &r)
{
	std::cout << "hello = hello\n";
	return *this;
}

Route::~Route()
{
	std::cout << "bye bye\n";
}
