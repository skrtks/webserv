cmake_minimum_required(VERSION 3.17)
project(webserv)

add_subdirectory(test)
add_subdirectory(libft)
include_directories(includes srcs getnextline libft)
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_FLAGS "-Wall -Werror -Wextra")

add_executable(webserv srcs/Base64.cpp
		srcs/Connection.cpp
		srcs/Location.cpp
		srcs/main.cpp
		srcs/parser.cpp
		srcs/RequestParser.cpp
		srcs/ResponseHandler.cpp
		srcs/Server.cpp
		srcs/Servermanager.cpp
		srcs/split.cpp
		srcs/utils.cpp
		includes/Base64.hpp
		includes/Cgi.hpp
		includes/Colours.hpp
		includes/Connection.hpp
		includes/libftGnl.hpp
		includes/Location.hpp
		includes/RequestParser.hpp
		includes/ResponseHandler.hpp
		includes/Server.hpp
		includes/Servermanager.hpp
		getnextline/get_next_line.cpp
		getnextline/get_next_line.hpp includes/Enums.hpp srcs/Cgi.cpp srcs/Enums.cpp)

target_link_libraries(webserv PUBLIC libft)
