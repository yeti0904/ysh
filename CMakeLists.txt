cmake_minimum_required(VERSION 3.10)
#set(CMAKE_VERBOSE_MAKEFILE on)

project(ysh VERSION 0.2)

include_directories(/usr/include/readline)
file(GLOB ysh_SRC CONFIGURE_DEPENDS "src/*.cc")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "-g -Wall -Wextra -pedantic")

add_executable(ysh ${ysh_SRC})
target_link_libraries(ysh readline)