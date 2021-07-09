# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: nelisabe <nelisabe@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/05/14 16:22:01 by nelisabe          #+#    #+#              #
#    Updated: 2021/07/09 13:48:56 by nelisabe         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

MAKEFLAGS =		--no-print-directory

### colors ###
RESET =			\033[0m
BOLD =			\033[1m
RED =			\033[31m
GREEN =			\033[32m
YELLOW =		\033[33m
##############

NAME =			webserv

COMP ?=			clang++

FLAGS ?=		-g #-Wall -Wextra -Werror -std=c++98

INCLUDES ?=		-I./server/ -I./http/ # delete

LIBS =			-L./server/ -lserv -L./http/ -lhttp
#SRC DIRS

SRC_DIR =		./

#SRC

SRC =			main.cpp

#OBJ DIRS

OBJ_DIR =		./temp/

#OBJ

OBJ =			$(addprefix $(OBJ_DIR), $(SRC:.cpp=.o))

D_FILES = 		$(patsubst %.o, %.d, $(OBJ))

###

all: libs $(OBJ_DIR) $(NAME)

client:	
	@$(COMP) client_t.cpp -o client

libs:
	@$(MAKE) -C ./server/

$(OBJ_DIR):
	@mkdir -p $(OBJ_DIR)

$(NAME): $(OBJ)
	@$(COMP) $(FLAGS) $< $(INCLUDES) $(LIBS) -o $@ -MMD
	@echo -e "$(YELLOW)$(BOLD)$(NAME) created!$(RESET)"

$(addprefix $(OBJ_DIR), %.o): %.cpp
	@$(COMP) -c $(FLAGS) $(INCLUDES) $< -o $@ -MMD

clean:
	@rm -rf temp
	@$(MAKE) clean -C ./server/

fclean: clean
	@rm -rf $(NAME)
	@$(MAKE) fclean -C ./server/
	@echo -e "$(RED)$(BOLD)$(NAME) deleted!$(RESET)"

re: fclean all

include $(wildcard $(D_FILES))

.PHONY: all, re, clean, fclean
