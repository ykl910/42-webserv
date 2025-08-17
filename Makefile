###########################          TARGET         ###########################

NAME                = webserv

###########################           FLAG          ###########################

CC                  = c++
FLAGS               = -g -Wall -Wextra -Werror -std=c++98

###########################          INCLUDE        ###########################

INCLUDE_DIR         := include
INCLUDE             := CGI.hpp \
                    Config.hpp \
                    Cookies.hpp \
                    DELETE.hpp \
                    Epoll.hpp \
                    Error.hpp \
                    GET.hpp \
                    HttpManager.hpp \
                    HttpRequest.hpp \
                    HttpResponse.hpp \
                    Poll.hpp \
                    POST.hpp \
                    Select.hpp \
                    Signal.hpp \
                    Socket.hpp \
                    textFormatting.hpp \
                    WebServ.hpp

INCLUDE             := $(addprefix $(INCLUDE_DIR)/, $(INCLUDE))

###########################          SOURCE         ###########################

SRC_DIR             := src
SRC                 := CGI.cpp \
                    Config.cpp \
                    ConfigParser.cpp \
                    ConfigUtils.cpp \
                    Cookies.cpp \
                    DELETE.cpp \
                    Epoll.cpp \
                    Error.cpp \
                    GET.cpp \
                    HttpManager.cpp \
                    HttpRequest.cpp \
                    HttpResponse.cpp \
                    main.cpp \
                    Poll.cpp \
                    POST.cpp \
                    Select.cpp \
                    Signal.cpp \
                    Socket.cpp \
                    utils.cpp

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

test:
	git submodule update --init --recursive

$(NAME): $(OBJ)
	$(CC) $(FLAGS) $^ $(LFLAGS) -o $@
	@echo "$(BOLD)$(ITALIC)$(GREEN)> $(NAME) creation successful!$(DEFAULT)"

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp $(INCLUDE)
	@mkdir -p $(dir $@)
	$(CC) $(FLAGS) -c $< -o $@

# PREPROCESSING
$(PP_DIR)/%.i: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CC) $(FLAGS) -E $< -o $@

# ASSEMBLY
$(ASM_DIR)/%.s: $(SRC_DIR)/%.cpp
	@mkdir -p $(dir $@)
	$(CC) $(FLAGS) -S $< -o $@

clean:
	$(RM) -rf $(OBJ) $(PP) $(ASM)
	@echo "$(BOLD)$(ITALIC)$(YELLOW)> $(NAME) objects removed!$(DEFAULT)"

fclean: clean
	$(RM) -rf $(OBJ_DIR) $(PP_DIR) $(ASM_DIR)
	$(RM) $(NAME)
	@echo "$(BOLD)$(ITALIC)$(YELLOW)> $(NAME) entirely cleaned!$(DEFAULT)"

re: fclean all

.PHONY: all pp asm test clean fclean re

###########################     TEXT FORMATTING     ###########################

DEFAULT=\033[0m
BOLD=\033[1m
ITALIC=\033[3m
GREEN=\033[32m
YELLOW=\033[33m
