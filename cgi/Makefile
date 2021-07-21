# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: nelisabe <nelisabe@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/07/21 16:20:10 by nelisabe          #+#    #+#              #
#    Updated: 2021/07/21 16:24:45 by nelisabe         ###   ########.fr        #
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

NAME =				libcgi.a

COMP ?=				clang++

FLAGS ?=			-g #-Wall -Wextra -Werror -std=c++98

INCLUDES ?=			-I. -I./../http/ -I./../config_parser/

#SRC DIRS

CGI_SRC_DIR =		./

#SRC

CGI_SRC =			Cgi.cpp

#OBJ DIRS

CGI_OBJ_DIR =		./temp/

#OBJ

CGI_OBJ =			$(addprefix $(CGI_OBJ_DIR), $(CGI_SRC:.cpp=.o))

D_FILES = 			$(patsubst %.o, %.d, $(CGI_OBJ))

###

all: $(CGI_OBJ_DIR) $(NAME)

$(CGI_OBJ_DIR):
	@mkdir -p $(CGI_OBJ_DIR)

$(NAME): $(CGI_OBJ)
	@ar rcs $(NAME) $^
	@echo -e "$(GREEN)$(BOLD)$(NAME) created!$(RESET)"

$(addprefix $(CGI_OBJ_DIR), %.o): %.cpp
	@$(COMP) -c $(FLAGS) $(INCLUDES) $< -o $@ -MMD

clean:
	@echo -e "$(GRAY)Deleting...$(RESET)"
	@rm -rf temp
	@echo -e "$(RED)$(BOLD)Cgi object files deleted!$(RESET)$(RED)$(RESET)"

fclean: clean
	@rm -rf $(NAME)
	@echo -e "$(RED)$(BOLD)$(NAME) deleted!$(RESET)"

re: fclean all

include $(wildcard $(D_FILES))

.PHONY: all, re, clean, fclean
