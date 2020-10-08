# **************************************************************************** #
#                                                                              #
#                                                         ::::::::             #
#    Makefile                                           :+:    :+:             #
#                                                      +:+                     #
#    By: sam <sam@student.codam.nl>                   +#+                      #
#                                                    +#+                       #
#    Created: 2020/10/02 15:16:50 by sam           #+#    #+#                  #
#    Updated: 2020/10/08 22:55:44 by pde-bakk      ########   odam.nl          #
#                                                                              #
# **************************************************************************** #

NAME = webserv
PARSE_DIR = ./Configparser
FILES = main parser split Server Servermanager Location
SRCS = $(addprefix srcs/, $(addsuffix .cpp, $(FILES)))
OBJS = $(SRCS:.cpp=.o)
INCLUDE = -Iincludes

CFLAGS = -Wall -Werror -Wextra -pedantic -Ofast
CXXFLAGS = -W -Wall -Werror -Wextra -pedantic -std=c++11 -Ofast
ifdef DEBUG
 CFLAGS += -g -fsanitize=address
 CXXFLAGS += -g -fsanitize=address
endif

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
	@$(CXX) $(CXXFLAGS) $(OBJS) $(GNL) $(LIBFT) $(INCLUDE) -o $@

%.a: %
	@echo $(ECHO) "$(PREFIX)$(GREEN) Compiling file $(END)$< $(GREEN)to $(END)$@"
	@make -s -C $<
	@cp $</$@ .
	
%.o: %.cpp
	@echo $(ECHO) "$(PREFIX)$(GREEN) Compiling file $(END)$< $(GREEN)to $(END)$@"
	@$(CXX) $(CXXFLAGS) $(INCLUDE) -c $< -o $@

.PHONY: clean fclean re all

clean:
	@echo $(ECHO) "$(PREFIX)$(GREEN) Removing .o files $(END)$(OUT_DIR)"
	@rm -f $(OBJS)
	@make clean -s -C getnextline
	@make clean -s -C libft

fclean: clean
	@echo $(ECHO) "$(PREFIX)$(GREEN) Removing executable $(END)$(OUT_DIR)"
	@rm -rf webservwebserv.dSYM *.a
	@make fclean -s -C getnextline
	@make fclean -s -C libft

re: fclean all

run: clean all
	./webserv
