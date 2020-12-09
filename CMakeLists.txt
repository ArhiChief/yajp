cmake_minimum_required(VERSION 3.17)

project(yajp
        VERSION 0.1
        DESCRIPTION "Yet Another JSON Processor"
        LANGUAGES C)

set(CMAKE_C_STANDARD 11)
set(CMAKE_VERBOSE_MAKEFILE 1)

enable_testing()

list(APPEND CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake")

option(YAJP_GENERATE_LEXER "Generate lexer using extern/lexer.c.re2c" TRUE)
option(YAJP_GENERATE_PARSER "Generate parser using extern/parser.y and extern/parser_template.c" TRUE)
option(YAJP_TRACK_STREAM "Force lexer to track scanned symbols. If parsing fails, error will contain line and column of bad token" ON)
set(YAJP_BUFFER_SIZE 32 CACHE STRING "Lexer buffer size and growing factor. Default and minimum value is 32 bytes")

add_subdirectory(src)
add_subdirectory(tests)


