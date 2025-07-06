cmake_minimum_required(VERSION 3.28)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(mmo LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

file(GLOB_RECURSE SOURCES CONFIGURE_DEPENDS
    ${CMAKE_CURRENT_SOURCE_DIR}/inc/mmo/*.h
    ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c
)

add_executable(server ${SOURCES})

target_include_directories(server PRIVATE 
    ${CMAKE_CURRENT_SOURCE_DIR}/inc
    ${CMAKE_CURRENT_SOURCE_DIR}
)

target_compile_options(server PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    -Wconversion
    -Wshadow
    -Wunused
    -Wswitch-enum
    -pedantic-errors
)

target_link_libraries(server -lm)
