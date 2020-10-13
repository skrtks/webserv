project(test)

set(CMAKE_CXX_STANDARD 11)
include_directories(../includes ../srcs)
add_executable(test testParseRequest.cpp ../srcs/RequestParser.cpp ../includes/RequestParser.hpp Catch2.h)