# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: algadea <algadea@student.42.fr>            +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2025/06/06 20:09:14 by algadea           #+#    #+#              #
#    Updated: 2025/06/07 13:41:49 by algadea          ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

###########################          TARGET         ###########################

NAME                = webserv

###########################           FLAG          ###########################

CC                  = c++
FLAGS               = -g -Wall -Wextra -Werror -std=c++98

###########################          INCLUDE        ###########################

INCLUDE_DIR         := include
INCLUDE             := webserv.hpp
INCLUDE             := $(addprefix $(INCLUDE_DIR)/, $(INCLUDE))

###########################          SOURCE         ###########################

SRC_DIR             := src
SRC                 := main.cpp
SRC                 := $(addprefix $(SRC_DIR)/, $(SRC))

###########################       COMPILATION       ###########################

PP_DIR              := pp
PP                  := $(patsubst $(SRC_DIR)/%.cpp, $(PP_DIR)/%.i, $(SRC))

ASM_DIR             := asm
ASM                 := $(patsubst $(SRC_DIR)/%.cpp, $(ASM_DIR)/%.s, $(SRC))

OBJ_DIR             := obj
OBJ                 := $(patsubst $(SRC_DIR)/%.cpp, $(OBJ_DIR)/%.o, $(SRC))

###########################          RULE           ###########################

all: $(NAME)

pp: $(PP)

asm: $(ASM)

$(NAME): $(OBJ)
	$(CC) $(FLAGS) $^ $(LFLAGS) -o $@
	@echo "$(GREEN)> $(NAME) creation successful!$(DEFAULT)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(INCLUDE)
	@mkdir -p $(dir $@)
	@$(CC) $(FLAGS) -c $< -o $@

# PREPROCESSING
$(PP_DIR)/%.i: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@$(CC) $(FLAGS) -E $< -o $@

# ASSEMBLY
$(ASM_DIR)/%.s: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	@$(CC) $(FLAGS) -S $< -o $@

clean:
	$(RM) -rf $(OBJ) $(PP) $(ASM)
	@echo "$(YELLOW)> $(NAME) objects removed!$(DEFAULT)"

fclean: clean
	$(RM) -rf $(OBJ_DIR) $(PP_DIR) $(ASM_DIR)
	$(RM) $(NAME)
	@echo "$(YELLOW)> $(NAME) entirely cleaned!$(DEFAULT)"

re: fclean all

.PHONY: all pp asm clean fclean re

###########################     ESCAPE SEQUENCE     ###########################

DEFAULT=\033[0m
BOLD=\033[1m
ITALIC=\033[3m

RED=\033[31m
GREEN=\033[32m
YELLOW=\033[33m
BLUE=\033[34m
MAGENTA=\033[35m