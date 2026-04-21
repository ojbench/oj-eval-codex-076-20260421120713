cmake_minimum_required(VERSION 3.10)
project(problem_076)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Avoid generating an executable named 'code' to prevent conflicts
add_executable(app076 code.cpp)
