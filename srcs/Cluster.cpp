/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Cluster.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: glaguyon           <skibidi@ohio.sus>      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 1833/02/30 06:67:85 by glaguyon          #+#    #+#             */
/*   Updated: 1833/02/30 06:67:85 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include "Cluster.hpp"

Cluster::Cluster()
{
	std::cout << "hello\n";
}

Cluster::Cluster(const Cluster &c)
{
	*this = c;
	std::cout << "hello hello\n";
}

Cluster &Cluster::operator=(const Cluster &c)
{
	std::cout << "hello = hello\n";
	return *this;
}

Cluster::~Cluster()
{
	std::cout << "bye bye\n";
}
