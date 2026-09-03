cmake_minimum_required(VERSION 3.20)
project(http-server LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS src/*.cpp)

add_executable(http-server ${SOURCES})
target_include_directories(http-server PRIVATE src/include)
