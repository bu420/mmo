cmake_minimum_required(VERSION 3.28)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

project(mmo LANGUAGES C)

set(CMAKE_C_STANDARD 23)
set(CMAKE_C_STANDARD_REQUIRED ON)

if(NOT CMAKE_BUILD_TYPE)
    set(CMAKE_BUILD_TYPE Debug)
endif()

set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/src/main.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/log.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/net.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/telnet.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/codegen.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/game.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/render.c
    ${CMAKE_CURRENT_SOURCE_DIR}/src/state/login.c
)

add_executable(server ${SOURCES})

target_include_directories(server PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/inc)

target_compile_options(server PRIVATE
    -Wall
    -Wextra
    -Wpedantic
    -Werror
    -Wconversion
    -Wshadow
    -Wunused
    -pedantic-errors
)

target_link_libraries(server -lm)
