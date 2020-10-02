# **************************************************************************** #
#                                                                              #
#                                                         ::::::::             #
#    Makefile                                           :+:    :+:             #
#                                                      +:+                     #
#    By: sam <sam@student.codam.nl>                   +#+                      #
#                                                    +#+                       #
#    Created: 2020/10/02 15:16:50 by sam           #+#    #+#                  #
#    Updated: 2020/10/02 19:23:17 by peerdb        ########   odam.nl          #
#                                                                              #
# **************************************************************************** #

NAME = webserv
SRCS = parser.cpp
CC = clang++
CFLAGS = -Wall -Werror -Wextra -pedantic -std=c++98
OBJS = $(SRCS:.cpp=.o)
GNL = getnextline.a
LIBFT = libft.a
ifeq ($(shell uname), Linux)
 ECHO = -e
endif

# Colours
DARK_GREEN	= \033[0;32m
GREEN		= \033[0;92m
END			= \033[0;0m

PREFIX		= $(DARK_GREEN)$(NAME) $(END)\xc2\xbb

all: $(NAME)

$(NAME): $(OBJS) $(LIBFT) $(GNL)
	@echo $(ECHO) "$(PREFIX)$(GREEN) Bundling executable... $(END)$(NAME)"
	@$(CC) $(CFLAGS) $(OBJS) $(GNL) $(LIBFT) -o $@

%.a: %
	@echo $@ depends on $<
	@echo $(ECHO) "$(PREFIX)$(GREEN) Compiling file $(END)$< $(GREEN)to $(END)$@"
	@make -s -C $<
	@cp $</$@ .
	
%.o: %.cpp
	@echo $(ECHO) "$(PREFIX)$(GREEN) Compiling file $(END)$< $(GREEN)to $(END)$@"
	@clang++ -Wall -Werror -Wextra -pedantic -std=c++98 -c $< -o $@

.PHONY: clean fclean re all

clean:
	@echo $(ECHO) "$(PREFIX)$(GREEN) Removing .o files $(END)$(OUT_DIR)"
	@rm -f $(OBJS) libft/*.o getnextline/*.o

fclean: clean
	@echo $(ECHO) "$(PREFIX)$(GREEN) Removing executable $(END)$(OUT_DIR)"
	@rm -f webserv
	@rm -rf webserv.dSYM *.a
	@make fclean -s -C getnextline
	@make fclean -s -C libft

re: fclean all
