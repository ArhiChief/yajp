add_executable(deserialization_tests deserialization_tests.c)

target_link_libraries(deserialization_tests
        PRIVATE yajp::test_common yajp::yajp_lib
        )

target_include_directories(deserialization_tests
        PRIVATE ${PROJECT_SOURCE_DIR}/src
        )

target_compile_definitions(deserialization_tests PUBLIC DEBUG)

add_test(NAME DeserializationTest1 COMMAND $<TARGET_FILE:deserialization_tests> 1)
add_test(NAME DeserializationTest2 COMMAND $<TARGET_FILE:deserialization_tests> 2)
add_test(NAME DeserializationTest3 COMMAND $<TARGET_FILE:deserialization_tests> 3)
add_test(NAME DeserializationTest4 COMMAND $<TARGET_FILE:deserialization_tests> 4)
add_test(NAME DeserializationTest5 COMMAND $<TARGET_FILE:deserialization_tests> 5)
add_test(NAME DeserializationTest6 COMMAND $<TARGET_FILE:deserialization_tests> 6)
add_test(NAME DeserializationTest7 COMMAND $<TARGET_FILE:deserialization_tests> 7)
add_test(NAME DeserializationTest8 COMMAND $<TARGET_FILE:deserialization_tests> 8)
add_test(NAME DeserializationTest9 COMMAND $<TARGET_FILE:deserialization_tests> 9)