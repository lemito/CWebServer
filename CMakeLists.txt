cmake_minimum_required(VERSION 3.27)
project(CWebServer C CXX)

set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 17)

add_executable(CWebServer main.cpp config.h)

add_library(responsesLib responses.c responses.h)
add_library(handlersLib handlers.c handlers.h)
add_library(loggerLib logger.c logger.h)
add_library(socketSetupLib socket_setup.c socket_setup.h)
add_library(file_handlingLib file_handling.c file_handling.h)
add_library(utilsLib utils.c utils.h)
add_library(html_helpersLib html_helpers.c html_helpers.h)

target_link_libraries(CWebServer responsesLib handlersLib loggerLib socketSetupLib file_handlingLib utilsLib html_helpersLib)