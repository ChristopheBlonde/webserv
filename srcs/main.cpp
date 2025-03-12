/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <cblonde@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/19 07:00:26 by cblonde           #+#    #+#             */
/*   Updated: 2025/03/12 16:18:51 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <webserv.hpp>
#include <Server.hpp>

volatile sig_atomic_t sig = 0;

void ft_exit(int signal)
{
	if (signal == SIGINT)
		sig = 1;
}

#include "Cluster.hpp"

int main(int argc, char **argv)
{
	//signal(SIGINT, ft_exit);

	try
	{
		Cluster	cluster((argc == 2) ? argv[1] : DEFAULT_CONF);

		cluster.startServers();
	}
	catch (std::exception &e)//XXX check if conf test route etc ? (origine de l'exception)
	{
		std::cerr << "error: " << e.what() << std::endl;
		return 1;
	}

//		Server test(8080);
//
//		test.init();
//		while (!sig)
//		{
//			test.run();
//			usleep(200);
//		}
	return (0);
}
