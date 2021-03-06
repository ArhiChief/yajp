/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * deserialization_tests.c
 * Copyright (C) 2020 Sergei Kosivchenko <arhichief@gmail.com> 
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy of this software and associated
 * documentation files (the "Software"), to deal in the Software without restriction, including without limitation the
 * rights to use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies of the Software, and to
 * permit persons to whom the Software is furnished to do so, subject to the following conditions:
 * 
 * The above copyright notice and this permission notice shall be included in all copies or substantial portions of
 * the Software.
 * 
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO
 * THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,
 * TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#include <stddef.h>
#include <stdbool.h>
#include <string.h>

#include "test_common.h"

#include "yajp/deserialization.h"
#include "yajp/deserialization_routine.h"

/* test cases prototypes */
static test_result_t yajp_deserialize_json_test_primitive_fields();
static test_result_t yajp_deserialize_json_test_string_fields();
static test_result_t yajp_deserialize_json_test_array_of_primitives_field();
static test_result_t yajp_deserialize_json_test_array_of_strings_field();
static test_result_t yajp_deserialize_json_test_matrix_of_primitives();
static test_result_t yajp_deserialize_json_test_cube_of_primitives();
static test_result_t yajp_deserialize_json_test_inherited_object();
static test_result_t yajp_deserialize_json_test_array_of_objects();
static test_result_t yajp_deserialize_json_test_full_example();

/* test suite declaration and initialization */
const test_case_t test_suite[] = {
        REGISTER_TEST_CASE(yajp_deserialize_json_test_primitive_fields, 1, yajp_deserialize_json_string, "where all JSON valuens are primitive"),
        REGISTER_TEST_CASE(yajp_deserialize_json_test_string_fields, 2, yajp_deserialize_json_string, "where JSON values are strings and some of them need allocation"),
        REGISTER_TEST_CASE(yajp_deserialize_json_test_array_of_primitives_field, 3, yajp_deserialize_json_string, "where JSON values are arrays of primitives and some of them need allocation"),
        REGISTER_TEST_CASE(yajp_deserialize_json_test_array_of_strings_field, 4, yajp_deserialize_json_string, "where JSON values are arrays of strings and some of them need allocation"),
        REGISTER_TEST_CASE(yajp_deserialize_json_test_matrix_of_primitives, 5, yajp_deserialize_json_string, "where JSON values are 2-dimension array of primitive values"),
        REGISTER_TEST_CASE(yajp_deserialize_json_test_cube_of_primitives, 6, yajp_deserialize_json_string, "where JSON values are 3-dimensions array of primitive values"),
        REGISTER_TEST_CASE(yajp_deserialize_json_test_inherited_object, 7, yajp_deserialize_json_string, "where JSON values are objects"),
        REGISTER_TEST_CASE(yajp_deserialize_json_test_array_of_objects, 8, yajp_deserialize_json_string, "where JSON values are arrays of objects"),
        REGISTER_TEST_CASE(yajp_deserialize_json_test_full_example, 9, yajp_deserialize_json_string, "with all possible combinations"),
};

/* test suite tests count declaration and initialization */
const long test_count = sizeof(test_suite) / sizeof(test_suite[0]);


static test_result_t yajp_deserialize_json_test_primitive_fields() {

    typedef struct {
        int int_field1;
        short short_field;
        double double_field;
        bool bool_field;
    } test_struct_t;

    static const char js[] = "{"
                             "  \"int_field1\":12345,"
                             "  \"double_field\":-1.33,"
                             "  \"other_field\":332,"
                             "  \"bool_field\":true,"
                             "  \"ignored_value\":0"
                             "}";
    static const size_t js_size = sizeof(js);

    yajp_deserialization_context_t ctx;

    int ret;
    yajp_deserialization_rule_t actions[4] = {0 };
    test_struct_t test_struct = {
            .int_field1 = INT16_MAX,
            .double_field = -332.112,
            .bool_field = false,
            .short_field = -122
    };

    // declare rules for test_struct_t.int_field1
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_HOLDER_TYPE   test_struct_t
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_NAME          int_field1
    #define YAJP_DESERIALIZATION_FIELD_TYPE                 (YAJP_DESERIALIZATION_TYPE_NUMBER)
    #define YAJP_DESERIALIZATION_SETTER                     yajp_set_int
    #define YAJP_DESERIALIZATION_RULE                       &actions[0]
    #define YAJP_DESERIALIZATION_RULE_INIT_RESULT           ret
    #include <yajp/deserialization_action_initialization.h>
    test_is_equal(ret, 0, "Failed to initialize action");
    // ==========================================

    // declare rules for test_struct_t.short_field
    #define YAJP_DESERIALIZATION_FIELD_NAME                 "other_field"
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_HOLDER_TYPE   test_struct_t
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_NAME          short_field
    #define YAJP_DESERIALIZATION_FIELD_TYPE                 (YAJP_DESERIALIZATION_TYPE_NUMBER)
    #define YAJP_DESERIALIZATION_SETTER                     yajp_set_short
    #define YAJP_DESERIALIZATION_RULE                       &actions[1]
    #define YAJP_DESERIALIZATION_RULE_INIT_RESULT           ret
    #include <yajp/deserialization_action_initialization.h>
    test_is_equal(ret, 0, "Failed to initialize action");
    // ==========================================

    // declare rules for test_struct_t.double_field
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_HOLDER_TYPE   test_struct_t
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_NAME          double_field
    #define YAJP_DESERIALIZATION_FIELD_TYPE                 (YAJP_DESERIALIZATION_TYPE_NUMBER)
    #define YAJP_DESERIALIZATION_SETTER                     yajp_set_double
    #define YAJP_DESERIALIZATION_RULE                       &actions[2]
    #define YAJP_DESERIALIZATION_RULE_INIT_RESULT           ret
    #include <yajp/deserialization_action_initialization.h>
    test_is_equal(ret, 0, "Failed to initialize action");
    // ==========================================

    // declare rules for test_struct_t.bool_field
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_HOLDER_TYPE   test_struct_t
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_NAME          bool_field
    #define YAJP_DESERIALIZATION_FIELD_TYPE                 (YAJP_DESERIALIZATION_TYPE_BOOLEAN)
    #define YAJP_DESERIALIZATION_SETTER                     yajp_set_bool
    #define YAJP_DESERIALIZATION_RULE                       &actions[3]
    #define YAJP_DESERIALIZATION_RULE_INIT_RESULT           ret
    #include <yajp/deserialization_action_initialization.h>
    test_is_equal(ret, 0, "Failed to initialize action");
    // ==========================================

    ret = yajp_deserialization_context_init(actions, ARR_LEN(actions), &ctx);
    test_is_equal(ret, 0, "Failed to initialize deserialization context");

    ret = yajp_deserialize_json_string(js, js_size, &ctx, &test_struct, NULL);
    test_is_equal(ret, 0, "Deserialization failed");

    test_is_equal(test_struct.int_field1, 12345, "Structure wasn't deserialized correctly");
    test_is_equal(test_struct.short_field, 332, "Structure wasn't deserialized correctly");
    test_is_equal_prec(test_struct.double_field, -1.33, DOUBLE_PRECISION, "Structure wasn't deserialized correctly");
    test_is_equal(test_struct.bool_field, true, "Structure wasn't deserialized correctly");

    return TEST_RESULT_PASSED;
}

static test_result_t yajp_deserialize_json_test_string_fields() {
#define str1    "test string"
#define str2    "hello world"
#define str3    "test test"
#define str4    "тестовая строка"

    typedef struct {
        char string1[ARR_LEN(str1)];
        char *string2;
        char string3[ARR_LEN(str3)];
        char *string4;
    } test_struct_t;

    static const char js[] = "{\"string1\":\""str1"\",\"other_string\":\""str2"\",\"string3\":\""str3"\",\"unused_string\":\"unused string\",\"other_string2\":\""str4"\"}";
    static const size_t js_size = sizeof(js);
    yajp_deserialization_context_t ctx;
    int ret;
    yajp_deserialization_rule_t actions[4] = {0 };
    test_struct_t test_struct = { 0 };

    // declare rules for test_struct_t.string1
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_HOLDER_TYPE   test_struct_t
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_NAME          string1
    #define YAJP_DESERIALIZATION_FIELD_TYPE                 (YAJP_DESERIALIZATION_TYPE_STRING)
    #define YAJP_DESERIALIZATION_SETTER                     yajp_set_string
    #define YAJP_DESERIALIZATION_RULE                       &actions[0]
    #define YAJP_DESERIALIZATION_RULE_INIT_RESULT           ret
    #include <yajp/deserialization_action_initialization.h>
    test_is_equal(ret, 0, "Failed to initialize action");
    // ==========================================

    // declare rules for test_struct_t.string2
    #define YAJP_DESERIALIZATION_FIELD_NAME                 "other_string"
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_HOLDER_TYPE   test_struct_t
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_NAME          string2
    #define YAJP_DESERIALIZATION_FIELD_TYPE                 (YAJP_DESERIALIZATION_TYPE_STRING)
    #define YAJP_DESERIALIZATION_OPTIONS                    (YAJP_DESERIALIZATION_OPTIONS_ALLOCATE)
    #define YAJP_DESERIALIZATION_SETTER                     yajp_set_string
    #define YAJP_DESERIALIZATION_RULE                       &actions[1]
    #define YAJP_DESERIALIZATION_RULE_INIT_RESULT           ret
    #include <yajp/deserialization_action_initialization.h>
    test_is_equal(ret, 0, "Failed to initialize action");
    // ==========================================

    // declare rules for test_struct_t.string3
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_HOLDER_TYPE   test_struct_t
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_NAME          string3
    #define YAJP_DESERIALIZATION_FIELD_TYPE                 (YAJP_DESERIALIZATION_TYPE_STRING)
    #define YAJP_DESERIALIZATION_SETTER                     yajp_set_string
    #define YAJP_DESERIALIZATION_RULE                       &actions[2]
    #define YAJP_DESERIALIZATION_RULE_INIT_RESULT           ret
    #include <yajp/deserialization_action_initialization.h>
    test_is_equal(ret, 0, "Failed to initialize action");
    // ==========================================

    // declare rules for test_struct_t.string4
    #define YAJP_DESERIALIZATION_FIELD_NAME                 "other_string2"
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_HOLDER_TYPE   test_struct_t
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_NAME          string4
    #define YAJP_DESERIALIZATION_FIELD_TYPE                 (YAJP_DESERIALIZATION_TYPE_STRING)
    #define YAJP_DESERIALIZATION_OPTIONS                    (YAJP_DESERIALIZATION_OPTIONS_ALLOCATE)
    #define YAJP_DESERIALIZATION_SETTER                     yajp_set_string
    #define YAJP_DESERIALIZATION_RULE                       &actions[3]
    #define YAJP_DESERIALIZATION_RULE_INIT_RESULT           ret
    #include <yajp/deserialization_action_initialization.h>
    test_is_equal(ret, 0, "Failed to initialize action");
    // ==========================================

    ret = yajp_deserialization_context_init(actions, ARR_LEN(actions), &ctx);
    test_is_equal(ret, 0, "Failed to initialize deserialization context");

    ret = yajp_deserialize_json_string(js, js_size, &ctx, &test_struct, NULL);
    test_is_equal(ret, 0, "Deserialization failed");

    test_is_equal(memcmp(test_struct.string1, str1, sizeof(str1)), 0, "test_struct.string1 deserialization failed. Found: '%s', expected: '%s", test_struct.string1, str1);
    test_is_equal(memcmp(test_struct.string2, str2, sizeof(str2)), 0, "test_struct.string2 deserialization failed. Found: '%s', expected: '%s", test_struct.string2, str2);
    test_is_equal(memcmp(test_struct.string3, str3, sizeof(str3)), 0, "test_struct.string3 deserialization failed. Found: '%s', expected: '%s", test_struct.string3, str3);
    test_is_equal(memcmp(test_struct.string4, str4, sizeof(str4)), 0, "test_struct.string4 deserialization failed. Found: '%s', expected: '%s", test_struct.string4, str4);

    free(test_struct.string2);
    free(test_struct.string4);

    return TEST_RESULT_PASSED;

#undef str1
#undef str2
#undef str3
#undef str4
}

// structure will represent our dynamic array
typedef struct array_handle array_handle_t;
struct array_handle {
    union {
        void *elems;
        array_handle_t *rows;
    };
    bool final_dim;
    size_t count;
};

typedef struct array_of_10_ints array_of_10_ints_t;
struct array_of_10_ints {
    union {
        int elems[10];
        array_of_10_ints_t *rows;
    };
    bool final_dim;
    size_t count;
};

static test_result_t yajp_deserialize_json_test_array_of_primitives_field() {
    typedef struct {
        array_handle_t arr1;
        array_handle_t *arr2;
        array_of_10_ints_t arr3;
        array_of_10_ints_t *arr4;

    } test_struct_t;

    static const char js[] = "{\"arr1\":[1,2,3,4,5,62],\"arr2\":[10,20,32,1,44,5,66],\"arr3\":[-1,-2,-3,-4,-5,-62],\"arr4\":[-10,-20,-32,-1,-44,-5,-66]}";
    static const size_t js_size = sizeof(js);

    static const int arr1[] = { 1, 2, 3, 4, 5, 62 };
    static const int arr2[] = { 10, 20, 32, 1, 44, 5, 66 };
    static const int arr3[] = { -1, -2, -3, -4, -5, -62 };
    static const int arr4[] = { -10, -20, -32, -1, -44, -5, -66 };

    yajp_deserialization_context_t ctx;
    int ret;
    yajp_deserialization_rule_t actions[4] = {0 };
    test_struct_t test_struct = { 0 };

    // declare rules for test_struct_t.arr1
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_HOLDER_TYPE   test_struct_t
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_NAME          arr1
    #define YAJP_DESERIALIZATION_FIELD_TYPE                 (YAJP_DESERIALIZATION_TYPE_ARRAY_OF | YAJP_DESERIALIZATION_TYPE_NUMBER)
    #define YAJP_DESERIALIZATION_OPTIONS                    (YAJP_DESERIALIZATION_OPTIONS_ALLOCATE_ELEMENTS)
    #define YAJP_DESERIALIZATION_SETTER                     yajp_set_int

    #define YAJP_DESERIALIZATION_ARRAY_ELEMENT_TYPE         int
    #define YAJP_DESERIALIZATION_ARRAY_ELEMENTS             elems
    #define YAJP_DESERIALIZATION_ARRAY_ROWS                 rows
    #define YAJP_DESERIALIZATION_ARRAY_COUNTER              count
    #define YAJP_DESERIALIZATION_ARRAY_FINAL_DIM            final_dim

    #define YAJP_DESERIALIZATION_RULE                     &actions[0]
    #define YAJP_DESERIALIZATION_RULE_INIT_RESULT         ret
    #include <yajp/deserialization_action_initialization.h>
    test_is_equal(ret, 0, "Failed to initialize action");
    // ==========================================

    // declare rules for test_struct_t.arr2
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_HOLDER_TYPE   test_struct_t
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_NAME          arr2
    #define YAJP_DESERIALIZATION_FIELD_TYPE                 (YAJP_DESERIALIZATION_TYPE_ARRAY_OF | YAJP_DESERIALIZATION_TYPE_NUMBER)
    #define YAJP_DESERIALIZATION_OPTIONS                    (YAJP_DESERIALIZATION_OPTIONS_ALLOCATE | YAJP_DESERIALIZATION_OPTIONS_ALLOCATE_ELEMENTS)
    #define YAJP_DESERIALIZATION_SETTER                     yajp_set_int

    #define YAJP_DESERIALIZATION_ARRAY_ELEMENT_TYPE         int
    #define YAJP_DESERIALIZATION_ARRAY_ELEMENTS             elems
    #define YAJP_DESERIALIZATION_ARRAY_ROWS                 rows
    #define YAJP_DESERIALIZATION_ARRAY_COUNTER              count
    #define YAJP_DESERIALIZATION_ARRAY_FINAL_DIM            final_dim

    #define YAJP_DESERIALIZATION_RULE                     &actions[1]
    #define YAJP_DESERIALIZATION_RULE_INIT_RESULT         ret
    #include <yajp/deserialization_action_initialization.h>
    test_is_equal(ret, 0, "Failed to initialize action");
    // ==========================================

    // declare rules for test_struct_t.arr3
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_HOLDER_TYPE   test_struct_t
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_NAME          arr3
    #define YAJP_DESERIALIZATION_FIELD_TYPE                 (YAJP_DESERIALIZATION_TYPE_ARRAY_OF | YAJP_DESERIALIZATION_TYPE_NUMBER)
    #define YAJP_DESERIALIZATION_SETTER                     yajp_set_int

    #define YAJP_DESERIALIZATION_ARRAY_ELEMENT_TYPE         int
    #define YAJP_DESERIALIZATION_ARRAY_ELEMENTS             elems
    #define YAJP_DESERIALIZATION_ARRAY_ROWS                 rows
    #define YAJP_DESERIALIZATION_ARRAY_COUNTER              count
    #define YAJP_DESERIALIZATION_ARRAY_FINAL_DIM            final_dim

    #define YAJP_DESERIALIZATION_RULE                     &actions[2]
    #define YAJP_DESERIALIZATION_RULE_INIT_RESULT         ret
    #include <yajp/deserialization_action_initialization.h>
    test_is_equal(ret, 0, "Failed to initialize action");
    // ==========================================

    // declare rules for test_struct_t.arr4
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_HOLDER_TYPE   test_struct_t
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_NAME          arr4
    #define YAJP_DESERIALIZATION_FIELD_TYPE                 (YAJP_DESERIALIZATION_TYPE_ARRAY_OF | YAJP_DESERIALIZATION_TYPE_NUMBER)
    #define YAJP_DESERIALIZATION_OPTIONS                    (YAJP_DESERIALIZATION_OPTIONS_ALLOCATE)
    #define YAJP_DESERIALIZATION_SETTER                     yajp_set_int

    #define YAJP_DESERIALIZATION_ARRAY_ELEMENT_TYPE         int
    #define YAJP_DESERIALIZATION_ARRAY_ELEMENTS             elems
    #define YAJP_DESERIALIZATION_ARRAY_ROWS                 rows
    #define YAJP_DESERIALIZATION_ARRAY_COUNTER              count
    #define YAJP_DESERIALIZATION_ARRAY_FINAL_DIM            final_dim

    #define YAJP_DESERIALIZATION_RULE                     &actions[3]
    #define YAJP_DESERIALIZATION_RULE_INIT_RESULT         ret
    #include <yajp/deserialization_action_initialization.h>
    test_is_equal(ret, 0, "Failed to initialize action");
    // ==========================================

    ret = yajp_deserialization_context_init(actions, ARR_LEN(actions), &ctx);
    test_is_equal(ret, 0, "Failed to initialize deserialization context");

    ret = yajp_deserialize_json_string(js, js_size, &ctx, &test_struct, NULL);
    test_is_equal(ret, 0, "Deserialization failed");

    test_is_equal(test_struct.arr1.count, ARR_LEN(arr1), "arr1 deserialization failed");
    test_is_true(test_struct.arr1.final_dim, "arr1 deserialization failed");
    test_is_equal(memcmp(test_struct.arr1.elems, arr1, sizeof(arr1)), 0, "arr1 deserialization failed");

    test_is_equal(test_struct.arr2->count, ARR_LEN(arr2), "arr2 deserialization failed");
    test_is_true(test_struct.arr2->final_dim, "arr2 deserialization failed");
    test_is_equal(memcmp(test_struct.arr2->elems, arr2, sizeof(arr2)), 0, "arr2 deserialization failed");

    test_is_equal(test_struct.arr3.count, ARR_LEN(arr3), "arr3 deserialization failed");
    test_is_true(test_struct.arr3.final_dim, "arr3 deserialization failed");
    test_is_equal(memcmp(test_struct.arr3.elems, arr3, sizeof(arr3)), 0, "arr3 deserialization failed");

    test_is_equal(test_struct.arr4->count, ARR_LEN(arr4), "arr4 deserialization failed");
    test_is_true(test_struct.arr4->final_dim, "arr4 deserialization failed");
    test_is_equal(memcmp(test_struct.arr4->elems, arr4, sizeof(arr4)), 0, "arr4 deserialization failed");

    free(test_struct.arr1.elems);
    free(test_struct.arr2->elems);
    free(test_struct.arr2);
    free(test_struct.arr4);

    return TEST_RESULT_PASSED;
}

static test_result_t yajp_deserialize_json_test_array_of_strings_field() {
#define str1 "test string 1"
#define str2 "test string 2"
#define str3 "test string 3"
#define str4 "test string 4"

    typedef struct {
        array_handle_t field;
    } test_struct_t;

    static const char js[] = "{ \"field\":[\""str1"\",\""str2"\",\""str3"\",\""str4"\"]}";
    static const size_t js_size = sizeof(js);
    static const char *test_arr[] = {str1, str2, str3, str4};
    static const size_t test_arr_sizes[] = { sizeof(str1), sizeof(str2), sizeof(str3), sizeof(str4) };

    yajp_deserialization_context_t ctx;
    int ret, i;
    size_t test_arr_elem_size;
    const char *test_arr_elem;
    char *test_struct_elem;
    yajp_deserialization_rule_t actions[1];
    test_struct_t test_struct = { 0 };

    // declare rules for test_struct_t.arr1
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_HOLDER_TYPE   test_struct_t
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_NAME          field
    #define YAJP_DESERIALIZATION_FIELD_TYPE                 (YAJP_DESERIALIZATION_TYPE_ARRAY_OF | YAJP_DESERIALIZATION_TYPE_STRING)
    #define YAJP_DESERIALIZATION_OPTIONS                    (YAJP_DESERIALIZATION_OPTIONS_ALLOCATE_ELEMENTS)
    #define YAJP_DESERIALIZATION_SETTER                     yajp_set_string

    #define YAJP_DESERIALIZATION_ARRAY_ELEMENT_TYPE         char *
    #define YAJP_DESERIALIZATION_ARRAY_ELEMENTS             elems
    #define YAJP_DESERIALIZATION_ARRAY_ROWS                 rows
    #define YAJP_DESERIALIZATION_ARRAY_COUNTER              count
    #define YAJP_DESERIALIZATION_ARRAY_FINAL_DIM            final_dim

    #define YAJP_DESERIALIZATION_RULE                     &actions[0]
    #define YAJP_DESERIALIZATION_RULE_INIT_RESULT         ret
    #include <yajp/deserialization_action_initialization.h>
    test_is_equal(ret, 0, "Failed to initialize action");
    // ==========================================

    ret = yajp_deserialization_context_init(actions, ARR_LEN(actions), &ctx);
    test_is_equal(ret, 0, "Failed to initialize deserialization context");

    ret = yajp_deserialize_json_string(js, js_size, &ctx, &test_struct, NULL);
    test_is_equal(ret, 0, "Deserialization failed");

    test_is_true(test_struct.field.final_dim, "");
    test_is_equal(test_struct.field.count, ARR_LEN(test_arr), "");

    for (i = 0; i < ARR_LEN(test_arr); i++) {
        test_arr_elem = test_arr[i];
        test_struct_elem = ((char **)test_struct.field.elems)[i];
        test_arr_elem_size = test_arr_sizes[i];

        test_is_equal(memcmp(test_arr_elem, test_struct_elem, test_arr_elem_size), 0, "");
        free(test_struct_elem);
    }

    free(test_struct.field.elems);

    return TEST_RESULT_PASSED;

#undef str1
#undef str2
#undef str3
#undef str4
}

static test_result_t yajp_deserialize_json_test_matrix_of_primitives() {
#define rows_cnt 5
#define column_cnt 5
    typedef struct {
        array_handle_t *arr;
    } test_struct_t;

    static const char js[] = "{ \"arr\":[[10,30,30,12,1],[-4,5,0,0,73],[34,-6,-3,4,62],[1,2,3,4,5],[10,9,8,7,6]]}";
    static const size_t js_size = sizeof(js);

    static const int test_arr[rows_cnt][column_cnt] = {
            { 10, 30, 30, 12, 1 },
            { -4, 5, 0, 0, 73 },
            { 34, -6, -3, 4, 62 },
            { 1, 2, 3, 4, 5 },
            { 10, 9, 8, 7, 6 }
    };
    test_struct_t test_struct;
    yajp_deserialization_context_t ctx;
    yajp_deserialization_rule_t actions[1];
    int i, j, ret;
    int test_arr_elem, test_struct_elem;

    // declare rules for test_struct_t.arr1
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_HOLDER_TYPE   test_struct_t
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_NAME          arr
    #define YAJP_DESERIALIZATION_FIELD_TYPE                 (YAJP_DESERIALIZATION_TYPE_ARRAY_OF | YAJP_DESERIALIZATION_TYPE_NUMBER)
    #define YAJP_DESERIALIZATION_OPTIONS                    (YAJP_DESERIALIZATION_OPTIONS_ALLOCATE_ELEMENTS | YAJP_DESERIALIZATION_OPTIONS_ALLOCATE)
    #define YAJP_DESERIALIZATION_SETTER                     yajp_set_int

    #define YAJP_DESERIALIZATION_ARRAY_ELEMENT_TYPE         int
    #define YAJP_DESERIALIZATION_ARRAY_ELEMENTS             elems
    #define YAJP_DESERIALIZATION_ARRAY_ROWS                 rows
    #define YAJP_DESERIALIZATION_ARRAY_COUNTER              count
    #define YAJP_DESERIALIZATION_ARRAY_FINAL_DIM            final_dim

    #define YAJP_DESERIALIZATION_RULE                     &actions[0]
    #define YAJP_DESERIALIZATION_RULE_INIT_RESULT         ret
    #include <yajp/deserialization_action_initialization.h>
    test_is_equal(ret, 0, "Failed to initialize action");
    // ==========================================

    ret = yajp_deserialization_context_init(actions, ARR_LEN(actions), &ctx);
    test_is_equal(ret, 0, "Failed to initialize deserialization context");

    ret = yajp_deserialize_json_string(js, js_size, &ctx, &test_struct, NULL);
    test_is_equal(ret, 0, "Deserialization failed");

    test_is_equal(test_struct.arr->count, rows_cnt, "Amount of rows are not equal");
    test_is_false(test_struct.arr->final_dim, "Deserialization is incorrect");

    for (i = 0; i < rows_cnt; i++) {
        test_is_equal(test_struct.arr->rows[i].count, column_cnt, "Column has different size");
        test_is_true(test_struct.arr->rows[i].final_dim, "Deserialization is incorrect");
        for (j = 0; j < column_cnt; j++) {
            test_arr_elem = test_arr[i][j];
            test_struct_elem = ((int *)test_struct.arr->rows[i].elems)[j];
            test_is_equal(test_arr_elem, test_struct_elem, "test_arr_elem[%d,%d] is not equal to test_struct.arr[%d, %d]. Expected: %d, Found: %d", i, j, i, j, test_arr_elem, test_struct_elem);
        }

        free(test_struct.arr->rows[i].elems);
    }

    free(test_struct.arr->rows);
    free(test_struct.arr);

    return TEST_RESULT_PASSED;
#undef rows_cnt
#undef column_cnt
}

static test_result_t yajp_deserialize_json_test_cube_of_primitives() {
    typedef struct {
        array_handle_t *arr;
    } test_struct_t;

    static const char js[] = "{\"arr\":[[[1,2,3],[4,5,6],[7,8,9]],[[10,11,12],[13,14,15],[16,17,18]],[[19,20,21],[22,23,24],[25,26,27]]]}";
    static const size_t js_size = sizeof(js);

#define dimI 3
#define dimJ 3
#define dimK 3
    static const int test_arr[dimI][dimJ][dimK] = {
            {
                { 1, 2, 3 },
                { 4, 5, 6 },
                { 7, 8, 9 }
            }, {
                { 10, 11, 12 },
                { 13, 14, 15 },
                { 16, 17, 18 }
            }, {
                { 19, 20, 21 },
                { 22, 23, 24 },
                { 25, 26, 27 }
            }
    };

    yajp_deserialization_context_t ctx;
    yajp_deserialization_rule_t actions[1];
    int i, j, k, test_arr_elem, test_struct_elem, ret;
    test_struct_t test_struct;

    // declare rules for test_struct_t.arr1
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_HOLDER_TYPE   test_struct_t
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_NAME          arr
    #define YAJP_DESERIALIZATION_FIELD_TYPE                 (YAJP_DESERIALIZATION_TYPE_ARRAY_OF | YAJP_DESERIALIZATION_TYPE_NUMBER)
    #define YAJP_DESERIALIZATION_OPTIONS                    (YAJP_DESERIALIZATION_OPTIONS_ALLOCATE_ELEMENTS | YAJP_DESERIALIZATION_OPTIONS_ALLOCATE)
    #define YAJP_DESERIALIZATION_SETTER                     yajp_set_int

    #define YAJP_DESERIALIZATION_ARRAY_ELEMENT_TYPE         int
    #define YAJP_DESERIALIZATION_ARRAY_ELEMENTS             elems
    #define YAJP_DESERIALIZATION_ARRAY_ROWS                 rows
    #define YAJP_DESERIALIZATION_ARRAY_COUNTER              count
    #define YAJP_DESERIALIZATION_ARRAY_FINAL_DIM            final_dim

    #define YAJP_DESERIALIZATION_RULE                     &actions[0]
    #define YAJP_DESERIALIZATION_RULE_INIT_RESULT         ret
    #include <yajp/deserialization_action_initialization.h>
    test_is_equal(ret, 0, "Failed to initialize action");
    // ==========================================

    ret = yajp_deserialization_context_init(actions, ARR_LEN(actions), &ctx);
    test_is_equal(ret, 0, "Failed to initialize deserialization context");

    ret = yajp_deserialize_json_string(js, js_size, &ctx, &test_struct, NULL);
    test_is_equal(ret, 0, "Deserialization failed");

    test_is_equal(test_struct.arr->count, dimI, "");
    test_is_false(test_struct.arr->final_dim, "");

    for (i = 0; i < dimI; i++) {
        test_is_equal(test_struct.arr->rows[i].count, dimJ, "");
        test_is_false(test_struct.arr->rows[i].final_dim, "");
        for (j = 0; j < dimJ; j++) {
            test_is_equal(test_struct.arr->rows[i].rows[j].count, dimK, "");
            test_is_true(test_struct.arr->rows[i].rows[j].final_dim, "");
            for (k = 0; k < dimK; k++) {
                test_struct_elem = ((int *)test_struct.arr->rows[i].rows[j].elems)[k];
                test_arr_elem = test_arr[i][j][k];
                test_is_equal(test_struct_elem, test_arr_elem, "");
            }
            free(test_struct.arr->rows[i].rows[j].elems);
        }
        free(test_struct.arr->rows[i].rows);
    }

    free(test_struct.arr->rows);
    free(test_struct.arr);

    return TEST_RESULT_PASSED;
#undef dimI
#undef dimJ
#undef dimK
}

typedef struct {
    int f1;
    array_handle_t f2;
} inner_object_t;

static test_result_t yajp_deserialize_json_test_inherited_object() {
    typedef struct {
        inner_object_t obj1;
        inner_object_t *obj2;
    } test_struct_t;

    static const char js[] = "{\"obj1\":{\"f1\":10,\"f2\":[1,2,3]},\"obj2\":{\"f1\":-2,\"f2\":[-1,-2,-3]}}";
    static const size_t js_size = sizeof(js);

    static const int test_arr1[] = {1,2,3};
    static const int test_arr2[] = {-1,-2,-3};

    yajp_deserialization_context_t ctx, inner_obj_ctx;
    yajp_deserialization_rule_t actions[2], inner_obj_actions[2];
    int ret;

    test_struct_t test_struct;
    int8_t zero_arr[sizeof(inner_object_t)/sizeof(int8_t)];

    memset(&test_struct, 0, sizeof (test_struct));
    memset(zero_arr, 0, sizeof(zero_arr));

    // declare rules for inner_object_t.f1
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_HOLDER_TYPE   inner_object_t
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_NAME          f1
    #define YAJP_DESERIALIZATION_FIELD_TYPE                 (YAJP_DESERIALIZATION_TYPE_NUMBER)
    #define YAJP_DESERIALIZATION_SETTER                     yajp_set_int
    #define YAJP_DESERIALIZATION_RULE                       &inner_obj_actions[0]
    #define YAJP_DESERIALIZATION_RULE_INIT_RESULT           ret
    #include <yajp/deserialization_action_initialization.h>
    test_is_equal(ret, 0, "Failed to initialize action");
    // ==========================================

    // declare rules for inner_object_t.f2
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_HOLDER_TYPE   inner_object_t
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_NAME          f2
    #define YAJP_DESERIALIZATION_FIELD_TYPE                 (YAJP_DESERIALIZATION_TYPE_ARRAY_OF | YAJP_DESERIALIZATION_TYPE_NUMBER)
    #define YAJP_DESERIALIZATION_OPTIONS                    (YAJP_DESERIALIZATION_OPTIONS_ALLOCATE_ELEMENTS)
    #define YAJP_DESERIALIZATION_SETTER                     yajp_set_int

    #define YAJP_DESERIALIZATION_ARRAY_ELEMENT_TYPE         int
    #define YAJP_DESERIALIZATION_ARRAY_ELEMENTS             elems
    #define YAJP_DESERIALIZATION_ARRAY_ROWS                 rows
    #define YAJP_DESERIALIZATION_ARRAY_COUNTER              count
    #define YAJP_DESERIALIZATION_ARRAY_FINAL_DIM            final_dim

    #define YAJP_DESERIALIZATION_RULE                       &inner_obj_actions[1]
    #define YAJP_DESERIALIZATION_RULE_INIT_RESULT           ret
    #include <yajp/deserialization_action_initialization.h>
    test_is_equal(ret, 0, "Failed to initialize action");
    // ==========================================

    ret = yajp_deserialization_context_init(inner_obj_actions, ARR_LEN(inner_obj_actions), &inner_obj_ctx);
    test_is_equal(ret, 0, "Failed to initialize deserialization context");

    // declare rules for test_struct_t.obj1
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_HOLDER_TYPE   test_struct_t
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_NAME          obj1
    #define YAJP_DESERIALIZATION_FIELD_TYPE                 (YAJP_DESERIALIZATION_TYPE_OBJECT)

    #define YAJP_DESERIALIZATION_OBJECT_CONTEXT             &inner_obj_ctx

    #define YAJP_DESERIALIZATION_RULE                     &actions[0]
    #define YAJP_DESERIALIZATION_RULE_INIT_RESULT         ret
    #include <yajp/deserialization_action_initialization.h>
    test_is_equal(ret, 0, "Failed to initialize action");
    // ==========================================

    // declare rules for test_struct_t.obj2
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_HOLDER_TYPE   test_struct_t
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_NAME          obj2
    #define YAJP_DESERIALIZATION_FIELD_TYPE                 (YAJP_DESERIALIZATION_TYPE_OBJECT)
    #define YAJP_DESERIALIZATION_OPTIONS                    (YAJP_DESERIALIZATION_OPTIONS_ALLOCATE)

    #define YAJP_DESERIALIZATION_OBJECT_CONTEXT             &inner_obj_ctx

    #define YAJP_DESERIALIZATION_RULE                       &actions[1]
    #define YAJP_DESERIALIZATION_RULE_INIT_RESULT           ret
    #include <yajp/deserialization_action_initialization.h>
    test_is_equal(ret, 0, "Failed to initialize action");
    // ==========================================

    ret = yajp_deserialization_context_init(actions, ARR_LEN(actions), &ctx);
    test_is_equal(ret, 0, "Failed to initialize deserialization context");

    ret = yajp_deserialize_json_string(js, js_size, &ctx, &test_struct, NULL);
    test_is_equal(ret, 0, "Deserialization failed");

    test_is_not_null(test_struct.obj2, "");
    test_is_not_equal(0, memcmp(&test_struct.obj1, zero_arr, sizeof(test_struct.obj1)), "");

    test_is_equal(10, test_struct.obj1.f1, "");
    test_is_equal(-2, test_struct.obj2->f1, "");

    test_is_equal(ARR_LEN(test_arr1), test_struct.obj1.f2.count, "");
    test_is_true(test_struct.obj1.f2.final_dim, "");
    test_is_equal(0, memcmp(test_arr1, test_struct.obj1.f2.elems, sizeof(test_arr1)), "");


    test_is_equal(ARR_LEN(test_arr2), test_struct.obj2->f2.count, "");
    test_is_true(test_struct.obj2->f2.final_dim, "");
    test_is_equal(0, memcmp(test_arr2, test_struct.obj2->f2.elems, sizeof(test_arr2)), "");

    free(test_struct.obj1.f2.elems);
    free(test_struct.obj2->f2.elems);
    free(test_struct.obj2);

    return TEST_RESULT_PASSED;
}

static test_result_t yajp_deserialize_json_test_array_of_objects() {
#define arr_cnt 3

    typedef struct {
        array_handle_t arr1;
    } test_struct_t;

    static const char js[] = "{\"arr1\":[{\"f1\":10,\"f2\":[1,2,3]},{\"f1\":-10,\"f2\":[-1,-2,-3]},{\"f1\":12,\"f2\":[11,21,31]}]}";
    static const size_t js_size = sizeof(js);

    static const int f1_vals[] = { 10, -10, 12 };
    static const int f2_vals[arr_cnt][arr_cnt] = { {1,2,3}, {-1,-2,-3}, {11,21,31} };
    inner_object_t *arr_elem;

    yajp_deserialization_context_t ctx, inner_obj_ctx;
    yajp_deserialization_rule_t actions[1], inner_obj_actions[2];
    int ret, i;
    test_struct_t test_struct;

    memset(&test_struct, 0, sizeof(test_struct));

    // declare rules for inner_object_t.f1
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_HOLDER_TYPE   inner_object_t
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_NAME          f1
    #define YAJP_DESERIALIZATION_FIELD_TYPE                 (YAJP_DESERIALIZATION_TYPE_NUMBER)
    #define YAJP_DESERIALIZATION_SETTER                     yajp_set_int
    #define YAJP_DESERIALIZATION_RULE                       &inner_obj_actions[0]
    #define YAJP_DESERIALIZATION_RULE_INIT_RESULT           ret
    #include <yajp/deserialization_action_initialization.h>
    test_is_equal(ret, 0, "Failed to initialize action");
    // ==========================================

    // declare rules for inner_object_t.f2
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_HOLDER_TYPE   inner_object_t
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_NAME          f2
    #define YAJP_DESERIALIZATION_FIELD_TYPE                 (YAJP_DESERIALIZATION_TYPE_ARRAY_OF | YAJP_DESERIALIZATION_TYPE_NUMBER)
    #define YAJP_DESERIALIZATION_OPTIONS                    (YAJP_DESERIALIZATION_OPTIONS_ALLOCATE_ELEMENTS)
    #define YAJP_DESERIALIZATION_SETTER                     yajp_set_int

    #define YAJP_DESERIALIZATION_ARRAY_ELEMENT_TYPE         int
    #define YAJP_DESERIALIZATION_ARRAY_ELEMENTS             elems
    #define YAJP_DESERIALIZATION_ARRAY_ROWS                 rows
    #define YAJP_DESERIALIZATION_ARRAY_COUNTER              count
    #define YAJP_DESERIALIZATION_ARRAY_FINAL_DIM            final_dim

    #define YAJP_DESERIALIZATION_RULE                     &inner_obj_actions[1]
    #define YAJP_DESERIALIZATION_RULE_INIT_RESULT         ret
    #include <yajp/deserialization_action_initialization.h>
    test_is_equal(ret, 0, "Failed to initialize action");
    // ==========================================

    ret = yajp_deserialization_context_init(inner_obj_actions, ARR_LEN(inner_obj_actions), &inner_obj_ctx);
    test_is_equal(ret, 0, "Failed to initialize deserialization context");

    // declare rules for test_struct_t.arr1
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_HOLDER_TYPE   test_struct_t
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_NAME          arr1
    #define YAJP_DESERIALIZATION_FIELD_TYPE                 (YAJP_DESERIALIZATION_TYPE_ARRAY_OF | YAJP_DESERIALIZATION_TYPE_OBJECT)
    #define YAJP_DESERIALIZATION_OPTIONS                    (YAJP_DESERIALIZATION_OPTIONS_ALLOCATE_ELEMENTS)

    #define YAJP_DESERIALIZATION_OBJECT_CONTEXT             &inner_obj_ctx

    #define YAJP_DESERIALIZATION_ARRAY_ELEMENT_TYPE         inner_object_t
    #define YAJP_DESERIALIZATION_ARRAY_ELEMENTS             elems
    #define YAJP_DESERIALIZATION_ARRAY_ROWS                 rows
    #define YAJP_DESERIALIZATION_ARRAY_COUNTER              count
    #define YAJP_DESERIALIZATION_ARRAY_FINAL_DIM            final_dim

    #define YAJP_DESERIALIZATION_RULE                     &actions[0]
    #define YAJP_DESERIALIZATION_RULE_INIT_RESULT         ret
    #include <yajp/deserialization_action_initialization.h>
    test_is_equal(ret, 0, "Failed to initialize action");
    // ==========================================

    ret = yajp_deserialization_context_init(actions, ARR_LEN(actions), &ctx);
    test_is_equal(ret, 0, "Failed to initialize deserialization context");

    ret = yajp_deserialize_json_string(js, js_size, &ctx, &test_struct, NULL);
    test_is_equal(ret, 0, "Deserialization failed");

    test_is_true(test_struct.arr1.final_dim, "");
    test_is_equal(test_struct.arr1.count, arr_cnt, "");

    for (i = 0; i < ARR_LEN(f1_vals); i++) {
        arr_elem = &((inner_object_t *)test_struct.arr1.elems)[i];
        test_is_equal(arr_elem->f1, f1_vals[i], "");
        test_is_true(arr_elem->f2.final_dim, "");
        test_is_equal(arr_elem->f2.count, arr_cnt, "");
        test_is_equal(0, memcmp(arr_elem->f2.elems, f2_vals[i], sizeof(f2_vals[i])), "");

        free(arr_elem->f2.elems);
    }

    free(test_struct.arr1.elems);

#undef arr_cnt
    return TEST_RESULT_PASSED;
}

static test_result_t yajp_deserialize_json_test_full_example() {
    typedef struct {
        int int_field;
        double other_field;
        bool bool_field;

        char string_field[ARR_LEN("test string") + 2];
        char *other_string;

        array_handle_t arr1;
        array_handle_t *arr2;
        array_of_10_ints_t arr3;
        array_of_10_ints_t *arr4;
        array_handle_t *arr5;
        array_handle_t two_dim_arr;

        inner_object_t object1;

        array_handle_t obj_arr;
    } test_struct_t;

    static const char js[] = "{\n"
                             "\t\"int_field\": \t123,\n"
                             "\t\"double_field\":\t22.332,\n"
                             "\t\"bool_field\":\tfalse,\n"
                             "\t\"string_field\":\t\"test string\",\n"
                             "\t\"other_string\":\t\"другая строка\",\n"
                             "\t\"arr1\":\t\t\t[1, 2, 3, 4],\n"
                             "\t\"arr2\":\t\t\t[5,6,7,8],\n"
                             "\t\"arr3\":\t\t\t[9,10,11,12,13],\n"
                             "\t\"arr4\":\t\t\t[14,15],\n"
                             "\t\"arr5\":\t\t\t[\"test string\", \"другая строка\"],\n"
                             "\t\"two_dim_arr\":\t[[1,2],[3,4]],\n"
                             "\t\"object1\":\t\t{\n"
                             "\t\t\"f1\":\t1,\n"
                             "\t\t\"f2\":\t[2,3,4]\n"
                             "\t},\n"
                             "\t\"ignored_field\": [5,6,7,8],\n"
                             "\t\"obj_arr\":\t[ {\"f1\": 3, \"f2\": [2,3,4]}, {\"f1\": 5, \"f2\": [4,4,4]} ]\n"
                             "}";
    static size_t js_size = sizeof(js);

    yajp_deserialization_context_t ctx, inner_obj_ctx;
    yajp_deserialization_rule_t actions[13], inner_obj_actions[2];
    int ret, i;
    test_struct_t test_struct;

    // declare rules for inner_object_t.f1
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_HOLDER_TYPE   inner_object_t
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_NAME          f1
    #define YAJP_DESERIALIZATION_FIELD_TYPE                 (YAJP_DESERIALIZATION_TYPE_NUMBER)
    #define YAJP_DESERIALIZATION_SETTER                     yajp_set_int
    #define YAJP_DESERIALIZATION_RULE                       &inner_obj_actions[0]
    #define YAJP_DESERIALIZATION_RULE_INIT_RESULT           ret
    #include <yajp/deserialization_action_initialization.h>
    test_is_equal(ret, 0, "Failed to initialize action");
    // ==========================================

    // declare rules for inner_object_t.f2
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_HOLDER_TYPE   inner_object_t
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_NAME          f2
    #define YAJP_DESERIALIZATION_FIELD_TYPE                 (YAJP_DESERIALIZATION_TYPE_ARRAY_OF | YAJP_DESERIALIZATION_TYPE_NUMBER)
    #define YAJP_DESERIALIZATION_OPTIONS                    (YAJP_DESERIALIZATION_OPTIONS_ALLOCATE_ELEMENTS)
    #define YAJP_DESERIALIZATION_SETTER                     yajp_set_int

    #define YAJP_DESERIALIZATION_ARRAY_ELEMENT_TYPE         int
    #define YAJP_DESERIALIZATION_ARRAY_ELEMENTS             elems
    #define YAJP_DESERIALIZATION_ARRAY_ROWS                 rows
    #define YAJP_DESERIALIZATION_ARRAY_COUNTER              count
    #define YAJP_DESERIALIZATION_ARRAY_FINAL_DIM            final_dim

    #define YAJP_DESERIALIZATION_RULE                       &inner_obj_actions[1]
    #define YAJP_DESERIALIZATION_RULE_INIT_RESULT           ret
    #include <yajp/deserialization_action_initialization.h>
    test_is_equal(ret, 0, "Failed to initialize action");
    // ==========================================

    ret = yajp_deserialization_context_init(inner_obj_actions, ARR_LEN(inner_obj_actions), &inner_obj_ctx);
    test_is_equal(ret, 0, "Failed to initialize deserialization context");

    // declare rules for test_struct_t.int_field
    #define YAJP_DESERIALIZATION_FIELD_NAME                 "int_field"
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_HOLDER_TYPE   test_struct_t
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_NAME          int_field
    #define YAJP_DESERIALIZATION_FIELD_TYPE                 (YAJP_DESERIALIZATION_TYPE_NUMBER)
    #define YAJP_DESERIALIZATION_SETTER                     yajp_set_int
    #define YAJP_DESERIALIZATION_RULE                       &actions[0]
    #define YAJP_DESERIALIZATION_RULE_INIT_RESULT           ret
    #include <yajp/deserialization_action_initialization.h>
    test_is_equal(ret, 0, "Failed to initialize action");
    // ==========================================

    // declare rules for test_struct_t.other_field
    #define YAJP_DESERIALIZATION_FIELD_NAME                 "double_field"
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_HOLDER_TYPE   test_struct_t
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_NAME          other_field
    #define YAJP_DESERIALIZATION_FIELD_TYPE                 (YAJP_DESERIALIZATION_TYPE_NUMBER)
    #define YAJP_DESERIALIZATION_SETTER                     yajp_set_double
    #define YAJP_DESERIALIZATION_RULE                       &actions[1]
    #define YAJP_DESERIALIZATION_RULE_INIT_RESULT           ret
    #include <yajp/deserialization_action_initialization.h>
    test_is_equal(ret, 0, "Failed to initialize action");
    // ==========================================

    // declare rules for test_struct_t.bool_field
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_HOLDER_TYPE   test_struct_t
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_NAME          bool_field
    #define YAJP_DESERIALIZATION_FIELD_TYPE                 (YAJP_DESERIALIZATION_TYPE_BOOLEAN)
    #define YAJP_DESERIALIZATION_SETTER                     yajp_set_bool
    #define YAJP_DESERIALIZATION_RULE                       &actions[2]
    #define YAJP_DESERIALIZATION_RULE_INIT_RESULT           ret
    #include <yajp/deserialization_action_initialization.h>
    test_is_equal(ret, 0, "Failed to initialize action");
    // ==========================================

    // declare rules for test_struct_t.string_field
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_HOLDER_TYPE   test_struct_t
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_NAME          string_field
    #define YAJP_DESERIALIZATION_FIELD_TYPE                 (YAJP_DESERIALIZATION_TYPE_STRING)
    #define YAJP_DESERIALIZATION_SETTER                     yajp_set_string
    #define YAJP_DESERIALIZATION_RULE                       &actions[3]
    #define YAJP_DESERIALIZATION_RULE_INIT_RESULT           ret
    #include <yajp/deserialization_action_initialization.h>
    test_is_equal(ret, 0, "Failed to initialize action");
    // ==========================================

    // declare rules for test_struct_t.string2
    #define YAJP_DESERIALIZATION_FIELD_NAME                 "other_string"
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_HOLDER_TYPE   test_struct_t
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_NAME          other_string
    #define YAJP_DESERIALIZATION_FIELD_TYPE                 (YAJP_DESERIALIZATION_TYPE_STRING)
    #define YAJP_DESERIALIZATION_OPTIONS                    (YAJP_DESERIALIZATION_OPTIONS_ALLOCATE)
    #define YAJP_DESERIALIZATION_SETTER                     yajp_set_string
    #define YAJP_DESERIALIZATION_RULE                       &actions[4]
    #define YAJP_DESERIALIZATION_RULE_INIT_RESULT           ret
    #include <yajp/deserialization_action_initialization.h>
    test_is_equal(ret, 0, "Failed to initialize action");
    // ==========================================

    // declare rules for test_struct_t.arr1
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_HOLDER_TYPE   test_struct_t
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_NAME          arr1
    #define YAJP_DESERIALIZATION_FIELD_TYPE                 (YAJP_DESERIALIZATION_TYPE_ARRAY_OF | YAJP_DESERIALIZATION_TYPE_NUMBER)
    #define YAJP_DESERIALIZATION_OPTIONS                    (YAJP_DESERIALIZATION_OPTIONS_ALLOCATE_ELEMENTS)
    #define YAJP_DESERIALIZATION_SETTER                     yajp_set_int

    #define YAJP_DESERIALIZATION_ARRAY_ELEMENT_TYPE         int
    #define YAJP_DESERIALIZATION_ARRAY_ELEMENTS             elems
    #define YAJP_DESERIALIZATION_ARRAY_ROWS                 rows
    #define YAJP_DESERIALIZATION_ARRAY_COUNTER              count
    #define YAJP_DESERIALIZATION_ARRAY_FINAL_DIM            final_dim

    #define YAJP_DESERIALIZATION_RULE                       &actions[5]
    #define YAJP_DESERIALIZATION_RULE_INIT_RESULT           ret
    #include <yajp/deserialization_action_initialization.h>
    test_is_equal(ret, 0, "Failed to initialize action");
    // ==========================================

    // declare rules for test_struct_t.arr2
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_HOLDER_TYPE   test_struct_t
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_NAME          arr2
    #define YAJP_DESERIALIZATION_FIELD_TYPE                 (YAJP_DESERIALIZATION_TYPE_ARRAY_OF | YAJP_DESERIALIZATION_TYPE_NUMBER)
    #define YAJP_DESERIALIZATION_OPTIONS                    (YAJP_DESERIALIZATION_OPTIONS_ALLOCATE | YAJP_DESERIALIZATION_OPTIONS_ALLOCATE_ELEMENTS)
    #define YAJP_DESERIALIZATION_SETTER                     yajp_set_int

    #define YAJP_DESERIALIZATION_ARRAY_ELEMENT_TYPE         int
    #define YAJP_DESERIALIZATION_ARRAY_ELEMENTS             elems
    #define YAJP_DESERIALIZATION_ARRAY_ROWS                 rows
    #define YAJP_DESERIALIZATION_ARRAY_COUNTER              count
    #define YAJP_DESERIALIZATION_ARRAY_FINAL_DIM            final_dim

    #define YAJP_DESERIALIZATION_RULE                       &actions[6]
    #define YAJP_DESERIALIZATION_RULE_INIT_RESULT           ret
    #include <yajp/deserialization_action_initialization.h>
    test_is_equal(ret, 0, "Failed to initialize action");
    // ==========================================

    // declare rules for test_struct_t.arr3
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_HOLDER_TYPE   test_struct_t
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_NAME          arr3
    #define YAJP_DESERIALIZATION_FIELD_TYPE                 (YAJP_DESERIALIZATION_TYPE_ARRAY_OF | YAJP_DESERIALIZATION_TYPE_NUMBER)
    #define YAJP_DESERIALIZATION_SETTER                     yajp_set_int

    #define YAJP_DESERIALIZATION_ARRAY_ELEMENT_TYPE         int
    #define YAJP_DESERIALIZATION_ARRAY_ELEMENTS             elems
    #define YAJP_DESERIALIZATION_ARRAY_ROWS                 rows
    #define YAJP_DESERIALIZATION_ARRAY_COUNTER              count
    #define YAJP_DESERIALIZATION_ARRAY_FINAL_DIM            final_dim

    #define YAJP_DESERIALIZATION_RULE                       &actions[7]
    #define YAJP_DESERIALIZATION_RULE_INIT_RESULT           ret
    #include <yajp/deserialization_action_initialization.h>
    test_is_equal(ret, 0, "Failed to initialize action");
    // ==========================================

    // declare rules for test_struct_t.arr4
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_HOLDER_TYPE   test_struct_t
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_NAME          arr4
    #define YAJP_DESERIALIZATION_FIELD_TYPE                 (YAJP_DESERIALIZATION_TYPE_ARRAY_OF | YAJP_DESERIALIZATION_TYPE_NUMBER)
    #define YAJP_DESERIALIZATION_OPTIONS                    (YAJP_DESERIALIZATION_OPTIONS_ALLOCATE)
    #define YAJP_DESERIALIZATION_SETTER                     yajp_set_int

    #define YAJP_DESERIALIZATION_ARRAY_ELEMENT_TYPE         int
    #define YAJP_DESERIALIZATION_ARRAY_ELEMENTS             elems
    #define YAJP_DESERIALIZATION_ARRAY_ROWS                 rows
    #define YAJP_DESERIALIZATION_ARRAY_COUNTER              count
    #define YAJP_DESERIALIZATION_ARRAY_FINAL_DIM            final_dim

    #define YAJP_DESERIALIZATION_RULE                       &actions[8]
    #define YAJP_DESERIALIZATION_RULE_INIT_RESULT           ret
    #include <yajp/deserialization_action_initialization.h>
    test_is_equal(ret, 0, "Failed to initialize action");
    // ==========================================

    // declare rules for test_struct_t.arr5
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_HOLDER_TYPE   test_struct_t
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_NAME          arr5
    #define YAJP_DESERIALIZATION_FIELD_TYPE                 (YAJP_DESERIALIZATION_TYPE_ARRAY_OF | YAJP_DESERIALIZATION_TYPE_STRING)
    #define YAJP_DESERIALIZATION_OPTIONS                    (YAJP_DESERIALIZATION_OPTIONS_ALLOCATE_ELEMENTS | YAJP_DESERIALIZATION_OPTIONS_ALLOCATE)
    #define YAJP_DESERIALIZATION_SETTER                     yajp_set_string

    #define YAJP_DESERIALIZATION_ARRAY_ELEMENT_TYPE         char *
    #define YAJP_DESERIALIZATION_ARRAY_ELEMENTS             elems
    #define YAJP_DESERIALIZATION_ARRAY_ROWS                 rows
    #define YAJP_DESERIALIZATION_ARRAY_COUNTER              count
    #define YAJP_DESERIALIZATION_ARRAY_FINAL_DIM            final_dim

    #define YAJP_DESERIALIZATION_RULE                       &actions[9]
    #define YAJP_DESERIALIZATION_RULE_INIT_RESULT           ret
    #include <yajp/deserialization_action_initialization.h>
    test_is_equal(ret, 0, "Failed to initialize action");
    // ==========================================

    // declare rules for test_struct_t.two_dim_arr
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_HOLDER_TYPE   test_struct_t
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_NAME          two_dim_arr
    #define YAJP_DESERIALIZATION_FIELD_TYPE                 (YAJP_DESERIALIZATION_TYPE_ARRAY_OF | YAJP_DESERIALIZATION_TYPE_NUMBER)
    #define YAJP_DESERIALIZATION_OPTIONS                    (YAJP_DESERIALIZATION_OPTIONS_ALLOCATE_ELEMENTS)
    #define YAJP_DESERIALIZATION_SETTER                     yajp_set_int

    #define YAJP_DESERIALIZATION_ARRAY_ELEMENT_TYPE         int
    #define YAJP_DESERIALIZATION_ARRAY_ELEMENTS             elems
    #define YAJP_DESERIALIZATION_ARRAY_ROWS                 rows
    #define YAJP_DESERIALIZATION_ARRAY_COUNTER              count
    #define YAJP_DESERIALIZATION_ARRAY_FINAL_DIM            final_dim

    #define YAJP_DESERIALIZATION_RULE                       &actions[10]
    #define YAJP_DESERIALIZATION_RULE_INIT_RESULT           ret
    #include <yajp/deserialization_action_initialization.h>
    test_is_equal(ret, 0, "Failed to initialize action");
    // ==========================================

    // declare rules for test_struct_t.object1
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_HOLDER_TYPE   test_struct_t
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_NAME          object1
    #define YAJP_DESERIALIZATION_FIELD_TYPE                 (YAJP_DESERIALIZATION_TYPE_OBJECT)

    #define YAJP_DESERIALIZATION_OBJECT_CONTEXT             &inner_obj_ctx

    #define YAJP_DESERIALIZATION_RULE                       &actions[11]
    #define YAJP_DESERIALIZATION_RULE_INIT_RESULT           ret
    #include <yajp/deserialization_action_initialization.h>
    test_is_equal(ret, 0, "Failed to initialize action");
    // ==========================================

    // declare rules for test_struct_t.obj_arr
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_HOLDER_TYPE   test_struct_t
    #define YAJP_DESERIALIZATION_STRUCT_FIELD_NAME          obj_arr
    #define YAJP_DESERIALIZATION_FIELD_TYPE                 (YAJP_DESERIALIZATION_TYPE_ARRAY_OF | YAJP_DESERIALIZATION_TYPE_OBJECT)
    #define YAJP_DESERIALIZATION_OPTIONS                    (YAJP_DESERIALIZATION_OPTIONS_ALLOCATE_ELEMENTS)

    #define YAJP_DESERIALIZATION_OBJECT_CONTEXT             &inner_obj_ctx

    #define YAJP_DESERIALIZATION_ARRAY_ELEMENT_TYPE         inner_object_t
    #define YAJP_DESERIALIZATION_ARRAY_ELEMENTS             elems
    #define YAJP_DESERIALIZATION_ARRAY_ROWS                 rows
    #define YAJP_DESERIALIZATION_ARRAY_COUNTER              count
    #define YAJP_DESERIALIZATION_ARRAY_FINAL_DIM            final_dim

    #define YAJP_DESERIALIZATION_RULE                       &actions[12]
    #define YAJP_DESERIALIZATION_RULE_INIT_RESULT           ret
    #include <yajp/deserialization_action_initialization.h>
    test_is_equal(ret, 0, "Failed to initialize action");
    // ==========================================

    ret = yajp_deserialization_context_init(actions, ARR_LEN(actions), &ctx);
    test_is_equal(ret, 0, "Failed to initialize deserialization context");

    memset(&test_struct, 0, sizeof(test_struct));

    ret = yajp_deserialize_json_string(js, js_size, &ctx, &test_struct, NULL);
    test_is_equal(ret, 0, "Deserialization failed");

    test_is_equal(123, test_struct.int_field, "");
    test_is_equal_prec(22.332, test_struct.other_field, DOUBLE_PRECISION, "");
    test_is_false(test_struct.bool_field, "");
    test_is_equal(0, memcmp(test_struct.string_field, "test string", sizeof("test string")), "");
    test_is_equal(0, memcmp(test_struct.other_string, "другая строка", sizeof("другая строка")), "");

    int arr1[] = {1, 2, 3, 4};
    test_is_true(test_struct.arr1.final_dim, "");
    test_is_equal(ARR_LEN(arr1), test_struct.arr1.count, "");
    test_is_equal(0, memcmp(test_struct.arr1.elems, arr1, sizeof(arr1)), "");

    int arr2[] = {5, 6, 7, 8};
    test_is_not_null(test_struct.arr2, "");
    test_is_true(test_struct.arr2->final_dim, "");
    test_is_equal(ARR_LEN(arr2), test_struct.arr2->count, "");
    test_is_equal(0, memcmp(test_struct.arr2->elems, arr2, sizeof(arr2)), "");

    int arr3[] = {9,10,11,12,13};
    test_is_true(test_struct.arr3.final_dim, "");
    test_is_equal(ARR_LEN(arr3), test_struct.arr3.count, "");
    test_is_equal(0, memcmp(test_struct.arr3.elems, arr3, sizeof(arr3)), "");

    int arr4[] = {14,15};
    test_is_not_null(test_struct.arr4, "");
    test_is_true(test_struct.arr4->final_dim, "");
    test_is_equal(ARR_LEN(arr4), test_struct.arr4->count, "");
    test_is_equal(0, memcmp(test_struct.arr4->elems, arr4, sizeof(arr4)), "");

    test_is_not_null(test_struct.arr5, "");
    test_is_true(test_struct.arr5->final_dim, "");
    test_is_equal(2, test_struct.arr5->count, "");

    char *str;

    str = ((char **)test_struct.arr5->elems)[0];
    test_is_equal(0, memcmp(str, "test string", sizeof("test string")), "");

    str = ((char **)test_struct.arr5->elems)[1];
    test_is_equal(0, memcmp(str, "другая строка", sizeof("другая строка")), "");

    int two_dim_arr[2][2] = {{1,2},{3,4}};
    test_is_false(test_struct.two_dim_arr.final_dim, "");
    test_is_equal(2, test_struct.two_dim_arr.count, "");
    array_handle_t *row ;
    for (i = 0; i < 2; i++) {
        row = &test_struct.two_dim_arr.rows[i];
        test_is_true(row->final_dim, "");
        test_is_equal(2, row->count, "");
        test_is_equal(0, memcmp(two_dim_arr[i], row->elems, row->count * sizeof(int)), "");
    }

    test_is_equal(1, test_struct.object1.f1, "");
    int obj_f2[] = {2,3,4};
    test_is_true(test_struct.object1.f2.final_dim, "");
    test_is_equal(ARR_LEN(obj_f2), test_struct.object1.f2.count, "");
    test_is_equal(0, memcmp(test_struct.object1.f2.elems, obj_f2, sizeof(obj_f2)), "");

    int obj_arr_f1[] = {3, 5};
    int obj_arr_f2[2][3] = { {2,3,4}, {4,4,4}};
    test_is_true(test_struct.obj_arr.final_dim, "");
    test_is_equal(2, test_struct.obj_arr.count, "");
    inner_object_t *inner_object;
    for (i = 0; i < 2; i++) {
        inner_object = &((inner_object_t *)test_struct.obj_arr.elems)[i];

        test_is_equal(obj_arr_f1[i], inner_object->f1, "");
        test_is_true(inner_object->f2.final_dim, "");
        test_is_equal(3, inner_object->f2.count, "");
        test_is_equal(0, memcmp(inner_object->f2.elems, obj_arr_f2[i], inner_object->f2.count * sizeof(int )), "");
    }

    return TEST_RESULT_PASSED;
}
