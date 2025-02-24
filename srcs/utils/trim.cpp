/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   trim.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <cblonde@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/23 10:04:10 by cblonde           #+#    #+#             */
/*   Updated: 2025/02/23 10:08:13 by cblonde          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

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
