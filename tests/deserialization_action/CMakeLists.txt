add_executable(deserialization_action_tests deserialization_action_tests.c)

target_link_libraries(deserialization_action_tests
        PRIVATE yajp::test_common yajp::yajp_lib
        )

target_include_directories(deserialization_action_tests
        PRIVATE ${PROJECT_SOURCE_DIR}/src
        )
