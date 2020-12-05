add_executable(deserialization_routine_tests deserialization_routines_tests.c)

target_link_libraries(deserialization_routine_tests
        PRIVATE yajp::test_common yajp::lib
        )

add_test(NAME DeserializationRoutinesTest1 COMMAND $<TARGET_FILE:deserialization_routine_tests> 1)