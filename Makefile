###########################          TARGET         ###########################

NAME                = webserv

###########################           FLAG          ###########################

CC                  = c++
FLAGS               = -g -Wall -Wextra -Werror -std=c++98

###########################          INCLUDE        ###########################

INCLUDE_DIR         := include
INCLUDE             := CGI.hpp \
                    Config.hpp \
                    Epoll.hpp \
                    Error.hpp \
                    HttpManager.hpp \
                    HttpRequest.hpp \
                    HttpResponse.hpp \
                    Poll.hpp \
                    Select.hpp \
                    Server.hpp \
                    signal.hpp \
                    Socket.hpp \
                    textFormatting.hpp \
                    WebServ.hpp

INCLUDE             := $(addprefix $(INCLUDE_DIR)/, $(INCLUDE))

###########################          SOURCE         ###########################

SRC_DIR             := src
SRC                 := config/Config-context.cpp \
                    config/Config-directive.cpp \
                    config/Config-print.cpp \
                    config/Config.cpp \
                    http/cgi/CGI-argv.cpp \
                    http/cgi/CGI-envp.cpp \
                    http/cgi/CGI-exec.cpp \
                    http/cgi/CGI.cpp \
                    http/method/DELETE.cpp \
                    http/method/GET.cpp \
                    http/method/POST.cpp \
                    http/request/HttpRequest.cpp \
                    http/response/autoindex.cpp \
                    http/response/HttpResponse.cpp \
                    http/response/resolution.cpp \
                    http/HttpManager.cpp \
                    multiplexer/Epoll.cpp \
                    multiplexer/Poll.cpp \
                    multiplexer/Select.cpp \
                    server/Server.cpp \
                    server/Socket.cpp \
                    utils/error.cpp \
                    utils/signal.cpp \
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
