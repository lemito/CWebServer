cmake_minimum_required(VERSION 3.27)
project(CWebServer C)

set(CMAKE_C_STANDARD 23)

add_executable(CWebServer main.c
        responses.c
        responses.h
        main.h
        handlers.c
        handlers.h
        logger.c
        logger.h
        config.h
        socket_setup.c
        socket_setup.h
        file_handling.c
        file_handling.h
        utils.c
        utils.h
        html_helpers.c
        html_helpers.h)
