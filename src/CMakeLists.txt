if (YAJP_GENERATE_LEXER)
    find_package(RE2C)
    if (NOT RE2C_FOUND)
        message(FATAL_ERROR "re2c executable wasn't found. Lexer generation is not possible")
    endif ()

    # re2c version used 2.0.3
    set(YAJP_LEXER "${CMAKE_CURRENT_BINARY_DIR}/lexer.c")
    set(YAJP_LEXER_SOURCE "${PROJECT_SOURCE_DIR}/extern/lexer.c.re2c")

    add_custom_command(
            OUTPUT lexer.c
            COMMAND ${RE2C_EXECUTABLE} -i ${YAJP_LEXER_SOURCE} -o ${YAJP_LEXER}
            DEPENDS ${YAJP_LEXER_SOURCE}
            COMMENT "Generating lexer from \"${YAJP_LEXER_SOURCE}\" using \"${RE2C_EXECUTABLE}\""
    )
else ()
    set(YAJP_LEXER "${CMAKE_CURRENT_SOURCE_DIR}/lexer.c")
endif ()

if (YAJP_GENERATE_PARSER)
    find_package(Lemon)
    if (NOT LEMON_FOUND)
        message(FATAL_ERROR "lemon executable didn't found. Parser generation is not possible")
    endif ()

    # parser_template.c and parser.y should work with lemon version 1.
    set(YAJP_PARSER "${CMAKE_CURRENT_BINARY_DIR}/parser.c")
    set(YAJP_PARSER_TEMPLATE "${PROJECT_SOURCE_DIR}/extern/parser_template.c")
    set(YAJP_PARSER_SOURCE "${PROJECT_SOURCE_DIR}/extern/parser.y")

    add_custom_command(
            OUTPUT parser.c
            COMMAND ${LEMON_EXECUTABLE} ${YAJP_PARSER_SOURCE} -q -T"${YAJP_PARSER_TEMPLATE}" -d${CMAKE_CURRENT_BINARY_DIR}
            COMMAND "${CMAKE_COMMAND}" -E rm -R "${CMAKE_CURRENT_BINARY_DIR}/parser.h"  # lemon also generates header file with tokens we don't need
            DEPENDS ${YAJP_PARSER_SOURCE} ${YAJP_PARSER_TEMPLATE}
            COMMENT "Generating parser from \"${YAJP_PARSER_SOURCE}\" and \"${YAJP_PARSER_TEMPLATE}\" using \"${LEMON_EXECUTABLE}\""
    )
else ()
    set(YAJP_PARSER "${CMAKE_CURRENT_SOURCE_DIR}/parser.c")
endif ()

set(YAJP_LIB_SOURCES
        deserialization.c
        lexer_misc.c
        deserialization_routine.c
        ${YAJP_LEXER}
        ${YAJP_PARSER}
        )

add_library(yajp_lib "${YAJP_LIB_SOURCES}")
add_library(yajp::lib ALIAS yajp_lib)

set_target_properties(yajp_lib PROPERTIES OUTPUT_NAME yajp)

target_include_directories(yajp_lib
        PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}
        PUBLIC
            $<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/include>
            $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        )

target_compile_definitions(yajp_lib
        PRIVATE $<$<CONFIG:Debug>:DEBUG>
        PUBLIC YAJP_BUFFER_SIZE=${YAJP_BUFFER_SIZE} $<$<BOOL:YAJP_TRACK_STREAM>:YAJP_TRACK_STREAM>
        )
