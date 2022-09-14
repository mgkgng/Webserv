# **************************************************************************** #
#                                                                              #
#                                                         :::      ::::::::    #
#    Makefile                                           :+:      :+:    :+:    #
#                                                     +:+ +:+         +:+      #
#    By: jrathelo <student.42nice.fr>               +#+  +:+       +#+         #
#                                                 +#+#+#+#+#+   +#+            #
#    Created: 2021/10/25 15:25:19 by jrathelo          #+#    #+#              #
#    Updated: 2022/09/14 20:05:34 by jrathelo         ###   ########.fr        #
#                                                                              #
# **************************************************************************** #

_BLACK			= \033[0;30m
_RED 			= \033[0;31m
_GREEN			= \033[0;32m
_BROWN			= \033[0;33m
_BLUE			= \033[0;34m
_PURPLE			= \033[0;35m
_CYAN			= \033[0;36m
_LIGHT_GRAY		= \033[0;37m
_DARK_GRAY		= \033[1;30m
_LIGHT_RED		= \033[1;31m
_LIGHT_GREEN	= \033[1;32m
_YELLOW			= \033[1;33m
_LIGHT_BLUE		= \033[1;34m
_LIGHT_PURPLE	= \033[1;35m
_LIGHT_CYAN		= \033[1;36m
_WHITE			= \033[1;37m
_NC 			= \033[0m

# Inverted, i.e. colored backgrounds
_IGREY			= \x1b[40m
_IRED			= \x1b[41m
_IGREEN			= \x1b[42m
_IYELLOW		= \x1b[43m
_IBLUE			= \x1b[44m
_IPURPLE		= \x1b[45
_ICYAN			= \x1b[46m
_IWHITE			= \x1b[47m

# Color reset
_COLOR_RESET	= \033[0m

# Folders
INCLUDES = -I./includes
SRC_DIR = src
OUTS = objs

# Source Files
SRC = main.cpp Request.cpp cgi.cpp Server.cpp Config.cpp Response.cpp utility.cpp mime.cpp

SRC_PLUS_PATH = $(addprefix $(SRC_DIR)/, $(SRC))

# Output Files
OUT = $(subst $(SRC_DIR)/, $(OUTS)/, $(patsubst %.cpp, %.opp, $(SRC_PLUS_PATH)))

NAME = webserv

CC = c++
CFLAGS = -Wall -Wextra -Werror -std=c++98 -g -O3 #-fsanitize=address

all : $(NAME)

$(NAME):  $(OUT)
	@echo "$(_PURPLE)Linking $(NAME)$(_COLOR_RESET)"
	@$(CC) $(CFLAGS) $(OUT) -o $(NAME) $(INCLUDES)
	@echo "$(_GREEN)DONE$(_COLOR_RESET)"


$(OUT): $(OUTS)/%.opp : $(SRC_DIR)/%.cpp
	@echo "$(_CYAN)Compiling $(basename $(notdir $*.opp)) $(_COLOR_RESET)"
	@mkdir -p $(@D)
	@$(CC) $(CFLAGS) -c $< -o $@ $(INCLUDES)

re: fclean
	@make $(NAME)

fclean: clean
	@echo "$(_RED)Cleaning output files$(_COLOR_RESET)"
	@rm -rf $(NAME)
	@rm -rf siege

clean:
	@echo "$(_RED)Cleaning object files$(_COLOR_RESET)"
	@rm -rf $(OUTS)

double_ports: fclean 
	@make && ./webserv config/double.json

siege:
	@curl -C - -O https://download.joedog.org/siege/siege-latest.tar.gz > /dev/null 2>&1
	@rm -rf siege > /dev/null 2>&1
	@mkdir siege > /dev/null 2>&1
	@tar -xvf siege-latest.tar.gz -C siege --strip-components 1 > /dev/null 2>&1
	@rm -rf  siege-latest.tar.gz > /dev/null 2>&1
	@cd siege && ./configure > /dev/null 2>&1
	@cd siege && make > /dev/null 2>&1

siege_site:
	@cd siege/src && ./siege --rc=../../.siegerc http://localhost:42069
	
.PHONY: clean fclean re all double_ports siege siege_site