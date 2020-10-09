# **************************************************************************** #
#                                                                              #
#                                                         ::::::::             #
#    Makefile                                           :+:    :+:             #
#                                                      +:+                     #
#    By: sam <sam@student.codam.nl>                   +#+                      #
#                                                    +#+                       #
#    Created: 2020/10/02 15:16:50 by sam           #+#    #+#                  #
#    Updated: 2020/10/09 15:18:50 by pde-bakk      ########   odam.nl          #
#                                                                              #
# **************************************************************************** #

NAME = webserv
FILES = main parser split Server Servermanager Location Connection RequestParser RequestHandler
SRCS = $(addprefix srcs/, $(addsuffix .cpp, $(FILES)))
OBJS = $(SRCS:.cpp=.o)
INCLUDE = -Iincludes

CFLAGS = -Wall -Werror -Wextra -pedantic -Ofast
CXXFLAGS = -W -Wall -Werror -Wextra -pedantic -std=c++11 -Ofast
ifdef DEBUG
 CFLAGS += -g -fsanitize=address -fno-omit-frame-pointer
 CXXFLAGS += -g -fsanitize=address -fno-omit-frame-pointer
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
