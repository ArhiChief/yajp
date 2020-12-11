cmake_minimum_required(VERSION 3.17)

project(yajp
        VERSION 0.1
        DESCRIPTION "Yet Another JSON Processor"
        LANGUAGES C)


set(CMAKE_VERBOSE_MAKEFILE 1)

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

option(YAJP_GENERATE_DOCS "Generate documentation for project" OFF)
option(YAJP_GENERATE_LEXER "Generate lexer using extern/lexer.c.re2c" TRUE)
option(YAJP_GENERATE_PARSER "Generate parser using extern/parser.y and extern/parser_template.c" TRUE)
option(YAJP_TRACK_STREAM "Force lexer to track scanned symbols. If parsing fails, error will contain line and column of bad token" OFF)
set(YAJP_BUFFER_SIZE 32 CACHE STRING "Lexer buffer size and growing factor. Default and minimum value is 32 bytes")

if (CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    set(CMAKE_C_STANDARD 11)

    include(GNUInstallDirs)

    if (YAJP_GENERATE_DOCS)
        find_package(Doxygen)

        if (Doxygen_FOUND)
            add_subdirectory(docs)
        else ()
            message(STATUS "Doxygen not found, not building docs")
        endif ()

    endif ()

endif ()

add_subdirectory(src)

if ((CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME OR YAJP_BUILD_TESTING) AND BUILD_TESTING)
    enable_testing()
    add_subdirectory(tests)
endif ()
