project(test)

set(CMAKE_CXX_STANDARD 11)
include_directories(../includes ../srcs)
add_executable(test  ../srcs/ParseRequest.cpp
               ../includes/ParseRequest.hpp
               test.cpp
               Catch2.h)