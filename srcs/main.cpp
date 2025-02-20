/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <cblonde@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/19 07:00:26 by cblonde           #+#    #+#             */
/*   Updated: 2025/02/20 17:21:03 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <webserv.hpp>
#include <Server.hpp>

int sig = 0;

void ft_exit(int signal)
{
	if (signal == SIGINT)
		sig = 1;
}

int main(void)
{
	signal(SIGINT, ft_exit);
	Server test(8080);

	test.init();
	while (!sig)
	{
		test.run();
		usleep(200);
	}
	return (0);
}
