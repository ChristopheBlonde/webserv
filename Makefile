NAME = webserv

SRC_DIR = srcs/
OBJ_DIR = obj/
HDR_DIR = includes/
SERV_DIR = server/

COMP = c++
CPPFLAGS = -Wall -Wextra -Werror -std=c++98 -g

SRC = main.cpp $(SERV_DIR)Server.cpp

OBJ = $(SRC:%.cpp=$(OBJ_DIR)%.o)

DEP = $(OBJ:%.o=%.d)

DEL = @echo -n "\033[2K\r";
GREEN = \033[0;32m
RED = \033[0;31m
BLUE = \033[0;34m
YELLOW = \033[0;33m
NOCOL = \033[m
BOLD = $(shell tput bold)
NOBOLD = $(shell tput sgr0)

MSG_CLEANING = "$(RED)$(BOLD)cleaning $(NAME)...$(NOBOLD)$(NOCOL)";
MSG_CLEANED = "$(RED)$(BOLD)cleaning done$(NOBOLD)$(NO_COLOR)";
MSG_COMPILING = "$(YELLOW)$(BOLD)compiling:$(NOBOLD)$(NOCOLOR) $(^)...";
MSG_READY = "$(BLUE)$(BOLD)$(NAME) ready$(NOBOLD)$(NOCOLOR)";


all : $(NAME)

$(NAME) : $(OBJ)
	@$(COMP) $(CPPFLAGS) $(OBJ) -I $(HDR_DIR) -o $(NAME)
	$(DEL)
	@echo $(MSG_READY)

-include $(DEP)

$(OBJ_DIR)%.o : $(SRC_DIR)%.cpp
	@ mkdir -p $(@D)
	$(DEL)
	@echo -n $(MSG_COMPILING)
	@ $(COMP) $(CPPFLAGS) -MMD -c $< -o $@ -I $(HDR_DIR)

clean :
	@echo $(MSG_CLEANING)
	@rm -rf $(OBJ_DIR)
	@echo $(MSG_CLEANED)

fclean : clean
	@rm -f $(NAME)

re : fclean all

.PHONY: all fclean clean re 
