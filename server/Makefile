# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: nelisabe <nelisabe@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/05/14 14:50:43 by nelisabe          #+#    #+#              #
#    Updated: 2021/07/23 09:41:45 by nelisabe         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

MAKEFLAGS =			--no-print-directory

### colors ###
RESET =				\033[0m
BOLD =				\033[1m
RED =				\033[31m
GREEN =				\033[32m
GRAY =				\033[2m
##############

NAME =				libserv.a

COMP ?=				clang++

FLAGS ?=			-g #-Wall -Wextra -Werror -std=c++98

INCLUDES ?=			-I. -I./../http/ -I./../config_parser/ -I./../cgi/

#SRC DIRS

SERV_SRC_DIR =		./

#SRC

SERV_SRC =			WebServer.cpp \
					SelectController.cpp \
					Server.cpp \
					Client.cpp \
					Errors.cpp

#OBJ DIRS

SERV_OBJ_DIR =		./temp/

#OBJ

SERV_OBJ =			$(addprefix $(SERV_OBJ_DIR), $(SERV_SRC:.cpp=.o))

D_FILES = 			$(patsubst %.o, %.d, $(SERV_OBJ))

###

all: $(SERV_OBJ_DIR) $(NAME)

$(SERV_OBJ_DIR):
	@mkdir -p $(SERV_OBJ_DIR)

$(NAME): $(SERV_OBJ)
	@ar rcs $(NAME) $^
	@echo -e "$(GREEN)$(BOLD)$(NAME) created!$(RESET)"

$(addprefix $(SERV_OBJ_DIR), %.o): %.cpp
	@$(COMP) -c $(FLAGS) $(INCLUDES) $< -o $@ -MMD

clean:
	@rm -rf temp
	@echo -e "$(RED)$(BOLD)Server object files deleted!$(RESET)$(RED)$(RESET)"

fclean: clean
	@rm -rf $(NAME)
	@echo -e "$(RED)$(BOLD)$(NAME) deleted!$(RESET)"

re: fclean all

include $(wildcard $(D_FILES))

.PHONY: all, re, clean, fclean
