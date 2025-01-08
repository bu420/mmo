cmake_minimum_required(VERSION 3.31)

project(mmo LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

add_executable(client ${CMAKE_CURRENT_SOURCE_DIR}/main.c)

target_compile_options(client PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    -Wconversion
)
