/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   webserv.hpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: cblonde <cblonde@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/02/19 07:03:14 by cblonde           #+#    #+#             */
/*   Updated: 2025/02/26 11:54:39 by cblonde          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef WEBSERV_HPP
# define WEBSERV_HPP

# include <iostream>
# include <fstream>
# include <algorithm>
# include <cstring>
# include <cerrno>
# include <csignal>
# include <vector>
# include <map>
# include <sstream>
# include <cctype>
# include <sys/types.h>
# include <sys/socket.h>
# include <poll.h>
# include <netinet/in.h>
# include <arpa/inet.h>
# include <unistd.h>
#include <sys/fcntl.h>

# define RED "\e[0;31m"
# define GREEN "\e[0;32m"
# define CYAN "\e[0;36m"
# define RESET "\e[0m"

# define INVALID_SOCKET -1
# define SOCKET_ERROR -1
# define PORT 8080

#endif
