add_executable(deserialization_tests deserialization_tests.c)

target_link_libraries(deserialization_tests
        PRIVATE yajp::test_common yajp::lib
        )

target_include_directories(deserialization_tests
        PRIVATE ${PROJECT_SOURCE_DIR}/src
        )

add_test(NAME DeserializationTest1 COMMAND $<TARGET_FILE:deserialization_tests> 1)