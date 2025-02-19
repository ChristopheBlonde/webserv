/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <cblonde@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/19 07:00:26 by cblonde           #+#    #+#             */
/*   Updated: 2025/02/19 17:03:23 by cblonde          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <webserv.hpp>

int sig = 0;

void ft_exit(int signal)
{
	if (signal == SIGINT)
		sig = 1;
}

int main(void)
{
	signal(SIGINT, ft_exit);

	/* create socket */

	long server = socket(AF_INET, SOCK_STREAM | SOCK_NONBLOCK, 0);
	if (server == INVALID_SOCKET)
	{
		std::cerr << RED << "Error: socket: " << (strerror(errno)) << RESET
			<< std::endl;
		return (1);
	}

	/* conf struct address */

	sockaddr_in	sin;
	std::memset(reinterpret_cast<char *>(&sin), 0, sizeof(sin));
	sin.sin_addr.s_addr = htonl(INADDR_ANY);
	sin.sin_family = AF_INET;
	sin.sin_port = htons(PORT);
	if (bind(server, reinterpret_cast<sockaddr *>(&sin),
				sizeof(sin)) == SOCKET_ERROR)
	{
		std::cerr << RED << "Error: bind: " << (strerror(errno)) << RESET
			<< std::endl;
		return (1);
	}
	if (listen(server, 100) == SOCKET_ERROR)
	{
		std::cerr << RED << "Error: bind: " << (strerror(errno)) << RESET
			<< std::endl;
		return (1);
	}
	long		client = -1;
	sockaddr_in	client_sin;
	socklen_t	size_csin = sizeof(client_sin);
	std::memset(reinterpret_cast<char *>(&client_sin), 0, size_csin);
	int n = 0;
	while (!sig)
	{
		pollfd poll_list[100];
		client = accept(server, reinterpret_cast<sockaddr *>(&client_sin),
				&size_csin);
		if (client == -1)
			continue ;
		poll_list[n++].fd = client;
		std::cout << GREEN << "lalai: " << poll_list[n].fd << std::endl;
		int reads = poll(poll_list, 100, -1);
		if (reads < 0)
			continue ;
		for (int i = 0; i < 100; i++)
		{
			char response[1024];
			if (poll_list[i].fd == -1)
				continue ;
			if (poll_list[i].revents & (POLL_ERR | POLL_HUP))
			{
				std::cerr << RED << "ERROR: poll: " << strerror(errno)
					<< RESET << std::endl;
				break ;
			}
				std::cout << RED << "test : i == " << i << RESET << std::endl;
			if (poll_list[i].revents & (POLL_IN))
			{
				int as_read = recv(poll_list[i].fd, response, 1023, 0);
				response[as_read] = '\0';
				std::cout << CYAN << response << RESET << std::endl;
			}
		}
		usleep(200);
	}
	close(server);
	close(client);
	return (0);
}
