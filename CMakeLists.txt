cmake_minimum_required(VERSION 3.27)
project(CWebServer C CXX)

set(CMAKE_C_STANDARD 23)
set(CMAKE_CXX_STANDARD 17)

add_subdirectory(libs/abseil)
set(ABSL_PROPAGATE_CXX_STD ON)

add_executable(CWebServer main.cpp config.h)

add_library(responsesLib STATIC responses.c
        responses.h)
add_library(handlersLib STATIC handlers.c
        handlers.h)
add_library(loggerLib STATIC logger.c)
add_library(socketSetupLib STATIC socket_setup.c)
add_library(file_handlingLib STATIC file_handling.c)
add_library(utilsLib STATIC utils.c)
add_library(html_helpersLib STATIC html_helpers.c)

target_link_libraries(handlersLib PRIVATE responsesLib)

target_link_libraries(CWebServer PRIVATE responsesLib handlersLib loggerLib socketSetupLib file_handlingLib utilsLib html_helpersLib absl::strings absl::log)