# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: nelisabe <nelisabe@student.42.fr>          +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/05/14 14:50:43 by nelisabe          #+#    #+#              #
#    Updated: 2021/07/25 21:49:16 by nelisabe         ###   ########.fr        #
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

NAME =				libconf.a

COMP ?=				clang++

FLAGS ?=			-g -std=c++98 -Wall -Wextra -Werror

INCLUDES ?=			-I. -I../http/

#SRC DIRS

CONF_SRC_DIR =		./

#SRC

CONF_SRC =			Location.cpp \
					WebservConf.cpp

#OBJ DIRS

CONF_OBJ_DIR =		./temp/

#OBJ

CONF_OBJ =			$(addprefix $(CONF_OBJ_DIR), $(CONF_SRC:.cpp=.o))

D_FILES = 			$(patsubst %.o, %.d, $(CONF_OBJ))

###

all: $(CONF_OBJ_DIR) $(NAME)

$(CONF_OBJ_DIR):
	@mkdir -p $(CONF_OBJ_DIR)

$(NAME): $(CONF_OBJ)
	@ar rcs $(NAME) $^
	@echo -e "$(GREEN)$(BOLD)$(NAME) created!$(RESET)"

$(addprefix $(CONF_OBJ_DIR), %.o): %.cpp
	@$(COMP) -c $(FLAGS) $(INCLUDES) $< -o $@ -MMD

clean:
	@rm -rf temp
	@echo -e "$(RED)$(BOLD)Config parser object files deleted!$(RESET)$(RED)$(RESET)"

fclean: clean
	@rm -rf $(NAME)
	@echo -e "$(RED)$(BOLD)$(NAME) deleted!$(RESET)"

re: fclean all

include $(wildcard $(D_FILES))

.PHONY: all, re, clean, fclean
