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
                    Epoll.hpp \
                    Error.hpp \
                    HttpManager.hpp \
                    HttpRequest.hpp \
                    HttpResponse.hpp \
                    Poll.hpp \
                    Select.hpp \
                    Server.hpp \
                    Signal.hpp \
                    Socket.hpp \
                    textFormatting.hpp \
                    WebServ.hpp

INCLUDE             := $(addprefix $(INCLUDE_DIR)/, $(INCLUDE))

###########################          SOURCE         ###########################

SRC_DIR             := src
SRC                 := config/Config.cpp \
                    config/ConfigUtils.cpp \
                    http/Cookies.cpp \
                    http/DELETE.cpp \
                    http/GET.cpp \
                    http/HttpManager.cpp \
                    http/HttpRequest.cpp \
                    http/HttpResponse.cpp \
                    http/POST.cpp \
                    http/Resolution.cpp \
                    multiplexer/Epoll.cpp \
                    multiplexer/Poll.cpp \
                    multiplexer/Select.cpp \
                    server/CGI.cpp \
                    server/Server.cpp \
                    server/Socket.cpp \
                    utils/Error.cpp \
                    utils/Signal.cpp \
                    utils/utils.cpp \
                    main.cpp

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
