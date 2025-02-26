/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   file.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <marvin@42.fr>                     +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/26 11:47:28 by cblonde           #+#    #+#             */
/*   Updated: 2025/02/26 12:32:14 by cblonde          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <webserv.hpp>

std::pair<int,std::string>	getFile(std::string path)
{
	std::ifstream				file;
	std::string					line;
	std::pair<int, std::string> result;

	file.open(path.c_str());
	if (!file.is_open())
	{
		std::cerr << RED << "Error: getFile: " << std::strerror(errno) << RESET
			<< std::endl;
		return (result);
	}
	while (getline(file, line))
		result.second += line;
	result.first = result.second.size();
	file.close();
	return (result);
}
