# tests should be run with command:
# ctest --build-and-test . cmake-build-debug --build-generator "Unix Makefiles" --test-command ctest --verbose

add_library(test_common OBJECT test_common.c)
add_library(yajp::test_common ALIAS test_common)
target_include_directories(test_common INTERFACE ${CMAKE_CURRENT_LIST_DIR})

add_subdirectory(lexer)
add_subdirectory(deserialization_routines)