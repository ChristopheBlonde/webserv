# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: cblonde <cblonde@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/02/19 06:52:36 by cblonde           #+#    #+#              #
#    Updated: 2025/02/19 07:02:48 by cblonde          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

CC = c++

CFLAGS = -Werror -Wextra -Wall -std=c++98

NAME = webserv

INCLUDES = -I./includes

SRCS = srcs/main.cpp

OBJS = $(SRCS:.cpp=.o)

%.o : %.cpp
	$(CC) $(CFLAGS) $(INCLUDES) -c $< -o $@

all : $(NAME)

$(NAME) : $(OBJS)
	$(CC) $(CFLAGS) $(INCLUDES) $(OBJS) -o $(NAME)

clean :
	rm -rf $(OBJS)

fclean : clean
	rm -rf $(NAME)

re : fclean $(NAME)

.PHONY : all clean fclean re
