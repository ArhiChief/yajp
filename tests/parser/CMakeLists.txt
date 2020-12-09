add_executable(parser_tests parser_tests.c)

target_link_libraries(parser_tests
        PRIVATE yajp::test_common yajp::lib)

target_include_directories(parser_tests PRIVATE ${PROJECT_SOURCE_DIR}/src)

add_test(NAME ParserTest1 COMMAND $<TARGET_FILE:parser_tests> 1)