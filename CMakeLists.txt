cmake_minimum_required(VERSION 3.17)
project(webserv)

add_subdirectory(test)
include_directories(includes srcs getnextline libft)
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_FLAGS "-pedantic -std=c++98 -Wall -Werror -Wextra")

add_executable(webserv getnextline/get_next_line.cpp
        getnextline/get_next_line.hpp
        includes/Connection.hpp
        includes/Location.hpp
        includes/RequestParser.hpp
        includes/Server.hpp
        includes/Servermanager.hpp
        includes/libftGnl.hpp
        libft/ft_atoi.c
        libft/ft_bzero.c
        libft/ft_calloc.c
        libft/ft_isalnum.c
        libft/ft_isalpha.c
        libft/ft_isascii.c
        libft/ft_isdigit.c
        libft/ft_isprint.c
        libft/ft_itoa.c
        libft/ft_memccpy.c
        libft/ft_memchr.c
        libft/ft_memcmp.c
        libft/ft_memcpy.c
        libft/ft_memmove.c
        libft/ft_memset.c
        libft/ft_putchar_fd.c
        libft/ft_putendl_fd.c
        libft/ft_putnbr_fd.c
        libft/ft_putstr_fd.c
        libft/ft_split.c
        libft/ft_strchr.c
        libft/ft_strdup.c
        libft/ft_strjoin.c
        libft/ft_strlcat.c
        libft/ft_strlcpy.c
        libft/ft_strlen.c
        libft/ft_strmapi.c
        libft/ft_strncmp.c
        libft/ft_strnstr.c
        libft/ft_strrchr.c
        libft/ft_strtrim.c
        libft/ft_substr.c
        libft/ft_tolower.c
        libft/ft_toupper.c
        libft/libft.h
        srcs/Connection.cpp
        srcs/Location.cpp
        srcs/main.cpp
        srcs/parser.cpp
        srcs/RequestParser.cpp
        srcs/Server.cpp
        srcs/Servermanager.cpp
        srcs/split.cpp
        includes/RequestHandler.hpp
        srcs/RequestHandler.cpp)