# **************************************************************************** #
#                                                                              #
#                                                         ::::::::             #
#    Makefile                                           :+:    :+:             #
#                                                      +:+                     #
#    By: sam <sam@student.codam.nl>                   +#+                      #
#                                                    +#+                       #
#    Created: 2020/10/02 15:16:50 by sam           #+#    #+#                  #
#    Updated: 2020/10/02 15:16:50 by sam           ########   odam.nl          #
#                                                                              #
# **************************************************************************** #

NAME = webserv
SRCS = main.cpp Connection.cpp ParseRequest.cpp
CC = clang++
CFLAGS = -Wall -Werror -Wextra -pedantic -std=c++98
OBJS = $(SRCS:.cpp=.o)

# Colours
DARK_GREEN	= \033[0;32m
GREEN		= \033[0;92m
END			= \033[0;0m

PREFIX		= $(DARK_GREEN)$(NAME) $(END)\xc2\xbb

all: $(NAME)

$(NAME): $(OBJS)
	@echo "$(PREFIX)$(GREEN) Bundling executable... $(END)$(NAME)"
	@$(CC) $(CFLAGS) $(OBJS) -o $@

asan: $(SRCS)
	@echo "$(PREFIX)$(GREEN) Bundling executable with address sanitizer support... $(END)$(NAME)"
	@$(CC) $(CFLAGS) -g -fsanitize=address -fno-omit-frame-pointer -O1 $(SRCS) -o $(NAME)


%.o: %.cpp
	@echo "$(PREFIX)$(GREEN) Compiling file $(END)$< $(GREEN)to $(END)$@"
	@clang++ -Wall -Werror -Wextra -pedantic -std=c++98 -c $< -o $@

.PHONY: clean fclean re all

clean:
	@echo "$(PREFIX)$(GREEN) Removing .o files $(END)$(OUT_DIR)"
	@rm -f $(OBJS)

fclean: clean
	@echo "$(PREFIX)$(GREEN) Removing executable $(END)$(OUT_DIR)"
	@rm -f webserv
	@rm -rf webserv.dSYM

re: fclean all