project(test)

set(CMAKE_CXX_STANDARD 11)
add_executable(test test.cpp ../ParseRequest.cpp ../ParseRequest.hpp Catch2.h)