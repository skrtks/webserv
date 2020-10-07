cmake_minimum_required(VERSION 3.17)
project(webserv)

add_subdirectory(test)
set(CMAKE_CXX_STANDARD 98)
set(CMAKE_CXX_FLAGS "-pedantic -std=c++98 -Wall -Werror -Wextra")

add_executable(webserv main.cpp Connection.cpp Connection.hpp ProcessRequest.cpp ProcessRequest.hpp)