add_executable(deserialization_tests deserialization_tests.c)

target_link_libraries(deserialization_tests
        PRIVATE yajp::test_common yajp::yajp_lib
        )

target_include_directories(deserialization_tests
        PRIVATE ${PROJECT_SOURCE_DIR}/src
        )

add_test(NAME DeserializationTest1 COMMAND $<TARGET_FILE:deserialization_tests> 1)
add_test(NAME DeserializationTest2 COMMAND $<TARGET_FILE:deserialization_tests> 2)
add_test(NAME DeserializationTest3 COMMAND $<TARGET_FILE:deserialization_tests> 3)
add_test(NAME DeserializationTest4 COMMAND $<TARGET_FILE:deserialization_tests> 4)