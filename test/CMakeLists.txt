project(test)

set(CMAKE_CXX_STANDARD 11)
add_executable(test testParseRequest.cpp ../RequestParser.cpp ../RequestParser.hpp Catch2.h)