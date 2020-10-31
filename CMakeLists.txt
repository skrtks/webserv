cmake_minimum_required(VERSION 3.17)
project(webserv)

add_subdirectory(test)
add_subdirectory(libft)
include_directories(includes srcs getnextline libft)
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_FLAGS "-Wall -Werror -Wextra")

add_executable(webserv getnextline/get_next_line.cpp
        getnextline/get_next_line.hpp
        includes/Connection.hpp
        includes/Location.hpp
        includes/RequestParser.hpp
        includes/Server.hpp
        includes/Servermanager.hpp
        includes/libftGnl.hpp
        srcs/Connection.cpp
        srcs/Location.cpp
        srcs/main.cpp
        srcs/parser.cpp
        srcs/RequestParser.cpp
        srcs/Server.cpp
        srcs/Servermanager.cpp
        srcs/split.cpp
        includes/ResponseHandler.hpp
        srcs/ResponseHandler.cpp)

target_link_libraries(webserv PUBLIC libft)