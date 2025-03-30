/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <cblonde@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/19 07:00:26 by cblonde           #+#    #+#             */
/*   Updated: 2025/03/30 22:29:59 by glaguyon         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <signal.h>
#include "Cluster.hpp"

volatile sig_atomic_t sig = 0;

void ft_exit(int signal)
{
	if (signal == SIGINT)
		sig = 1;
}

int main(int argc, char **argv)
{
	if (signal(SIGINT, ft_exit) == SIG_ERR)
	{
		std::cerr << "error: signal\n";
		return 1;
	}
	if (argc > 2)
	{
		std::cerr << "usage: ./webserv [conf_file]\n";
		return 1;
	}
	try
	{
		Cluster	cluster((argc >= 2) ? argv[1] : DEFAULT_CONF);

		cluster.startServers();
		while (!sig)
			cluster.run();
	}
	catch (std::exception &e)
	{
		std::cerr << "error: " << e.what() << std::endl;
		return 1;
	}
	return (0);
}
