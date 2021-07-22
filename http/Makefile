# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: nelisabe <nelisabe@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/05/14 14:50:43 by nelisabe          #+#    #+#              #
#    Updated: 2021/07/22 10:37:49 by nelisabe         ###   ########.fr        #
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

NAME =				libhttp.a

COMP ?=				clang++

FLAGS ?=			-g #-Wall -Wextra -Werror -std=c++98

INCLUDES ?=			-I. -I./../config_parser/

#SRC DIRS

HTTP_SRC_DIR =		./

#SRC

HTTP_SRC =			header_handlers.cpp \
					http.cpp \
					url.cpp \
					utility_http.cpp \
					errors.cpp

#OBJ DIRS

HTTP_OBJ_DIR =		./temp/

#OBJ

HTTP_OBJ =			$(addprefix $(HTTP_OBJ_DIR), $(HTTP_SRC:.cpp=.o))

D_FILES = 			$(patsubst %.o, %.d, $(HTTP_OBJ))

###

all: $(HTTP_OBJ_DIR) $(NAME)

$(HTTP_OBJ_DIR):
	@mkdir -p $(HTTP_OBJ_DIR)

$(NAME): $(HTTP_OBJ)
	@ar rcs $(NAME) $^
	@echo -e "$(GREEN)$(BOLD)$(NAME) created!$(RESET)"

$(addprefix $(HTTP_OBJ_DIR), %.o): %.cpp
	@$(COMP) -c $(FLAGS) $(INCLUDES) $< -o $@ -MMD

clean:
	@rm -rf temp
	@echo -e "$(RED)$(BOLD)Http object files deleted!$(RESET)$(RED)$(RESET)"

fclean: clean
	@rm -rf $(NAME)
	@echo -e "$(RED)$(BOLD)$(NAME) deleted!$(RESET)"

re: fclean all

include $(wildcard $(D_FILES))

.PHONY: all, re, clean, fclean
