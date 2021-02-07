add_executable(lexer_tests lexer_tests.c)

target_include_directories(lexer_tests PRIVATE ${PROJECT_SOURCE_DIR}/src)

target_link_libraries(lexer_tests
        PRIVATE yajp::test_common yajp::yajp_lib
        )

target_include_directories(lexer_tests PUBLIC ${PROJECT_BINARY_DIR}/src)

add_test(NAME LexerTest1 COMMAND $<TARGET_FILE:lexer_tests> 1)
add_test(NAME LexerTest2 COMMAND $<TARGET_FILE:lexer_tests> 2)
add_test(NAME LexerTest3 COMMAND $<TARGET_FILE:lexer_tests> 3)
add_test(NAME LexerTest4 COMMAND $<TARGET_FILE:lexer_tests> 4)
add_test(NAME LexerTest5 COMMAND $<TARGET_FILE:lexer_tests> 5)
add_test(NAME LexerTest6 COMMAND $<TARGET_FILE:lexer_tests> 6)
add_test(NAME LexerTest7 COMMAND $<TARGET_FILE:lexer_tests> 7)
add_test(NAME LexerTest8 COMMAND $<TARGET_FILE:lexer_tests> 8)
add_test(NAME LexerTest9 COMMAND $<TARGET_FILE:lexer_tests> 9)
add_test(NAME LexerTest10 COMMAND $<TARGET_FILE:lexer_tests> 10)
add_test(NAME LexerTest11 COMMAND $<TARGET_FILE:lexer_tests> 11)
add_test(NAME LexerTest12 COMMAND $<TARGET_FILE:lexer_tests> 12)
add_test(NAME LexerTest13 COMMAND $<TARGET_FILE:lexer_tests> 13)
add_test(NAME LexerTest14 COMMAND $<TARGET_FILE:lexer_tests> 14)
add_test(NAME LexerTest15 COMMAND $<TARGET_FILE:lexer_tests> 15)
add_test(NAME LexerTest16 COMMAND $<TARGET_FILE:lexer_tests> 16)
add_test(NAME LexerTest17 COMMAND $<TARGET_FILE:lexer_tests> 17)