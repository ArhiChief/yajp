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


/* test suite declaration and initialization */
const test_case_t test_suite[] = {
        REGISTER_TEST_CASE(yajp_deserialize_json_test_primitive_fields, 1, yajp_deserialize_json_string, "where all JSON valuens are primitive"),
        REGISTER_TEST_CASE(yajp_deserialize_json_test_string_fields, 2, yajp_deserialize_json_string, "where JSON values are strings and some of them need allocation"),
        REGISTER_TEST_CASE(yajp_deserialize_json_test_array_of_primitives_field, 3, yajp_deserialize_json_string, "where JSON values are arrays of primitives and some of them need allocation"),
        REGISTER_TEST_CASE(yajp_deserialize_json_test_array_of_strings_field, 4, yajp_deserialize_json_string, "where JSON values are arrays of strings and some of them need allocation"),
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

    static const char js[] = "{\"int_field1\":12345,\"double_field\":-1.33,\"other_field\":332,\"bool_field\":true,\"ignored_value\":0}";
    static const size_t js_size = sizeof(js);

    yajp_deserialization_ctx_t ctx;
    yajp_deserialization_result_t dres;
    int ret;
    yajp_deserialization_action_t actions[4] = { 0 };
    test_struct_t test_struct = {
            .int_field1 = INT16_MAX,
            .double_field = -332.112,
            .bool_field = false,
            .short_field = -122
    };

    ret = YAJP_PRIMITIVE_FIELD_DESERIALIZATION_ACTION_INIT(test_struct_t, int_field1, yajp_set_int, &actions[0]);
    test_is_equal(ret, 0, "Failed to initialize action");

    ret = YAJP_PRIMITIVE_FIELD_OVERWRITE_DESERIALIZATION_ACTION_INIT("other_field", test_struct_t, short_field,
                                                                     yajp_set_short, &actions[1]);
    test_is_equal(ret, 0, "Failed to initialize action");

    ret = YAJP_PRIMITIVE_FIELD_DESERIALIZATION_ACTION_INIT(test_struct_t, double_field, yajp_set_double, &actions[2]);
    test_is_equal(ret, 0, "Failed to initialize action");

    ret = YAJP_PRIMITIVE_FIELD_DESERIALIZATION_ACTION_INIT(test_struct_t, bool_field, yajp_set_bool, &actions[3]);
    test_is_equal(ret, 0, "Failed to initialize action");

    yajp_deserialization_ctx_init(actions, ARR_LEN(actions), &ctx);

    dres = yajp_deserialize_json_string(js, js_size, &ctx, &test_struct, NULL);

    test_is_equal(dres.status, YAJP_DESERIALIZATION_RESULT_STATUS_OK, "Deserialization failed");
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
    yajp_deserialization_ctx_t ctx;
    yajp_deserialization_result_t dres;
    int ret;
    yajp_deserialization_action_t actions[4] = { 0 };
    test_struct_t test_struct = { 0 };

    ret = YAJP_STRING_FIELD_DESERIALIZATION_ACTION_INIT(test_struct_t, string1, yajp_set_string, false, &actions[0]);
    test_is_equal(ret, 0, "Failed to initialize action");

    ret = YAJP_STRING_FIELD_OVERWRITE_DESERIALIZATION_ACTION_INIT("other_string", test_struct_t, string2, yajp_set_string, true, &actions[1]);
    test_is_equal(ret, 0, "Failed to initialize action");

    ret = YAJP_STRING_FIELD_DESERIALIZATION_ACTION_INIT(test_struct_t, string3, yajp_set_string, false, &actions[2]);
    test_is_equal(ret, 0, "Failed to initialize action");

    ret = YAJP_STRING_FIELD_OVERWRITE_DESERIALIZATION_ACTION_INIT("other_string2", test_struct_t, string4, yajp_set_string, true, &actions[3]);
    test_is_equal(ret, 0, "Failed to initialize action");

    yajp_deserialization_ctx_init(actions, ARR_LEN(actions), &ctx);

    dres = yajp_deserialize_json_string(js, js_size, &ctx, &test_struct, NULL);

    test_is_equal(dres.status, YAJP_DESERIALIZATION_RESULT_STATUS_OK, "Deserialization failed");

    test_is_equal(memcmp(test_struct.string1, str1, sizeof(str1)), 0, "test_struct.string1 deserialization failed. Found: '%s', expected: '%s", test_struct.string1, str1);
    test_is_equal(memcmp(test_struct.string2, str2, sizeof(str2)), 0, "test_struct.string2 deserialization failed. Found: '%s', expected: '%s", test_struct.string2, str2);
    test_is_equal(memcmp(test_struct.string3, str3, sizeof(str3)), 0, "test_struct.string3 deserialization failed. Found: '%s', expected: '%s", test_struct.string3, str3);
    test_is_equal(memcmp(test_struct.string4, str4, sizeof(str4)), 0, "test_struct.string4 deserialization failed. Found: '%s', expected: '%s", test_struct.string4, str4);

    return TEST_RESULT_PASSED;
}

static test_result_t yajp_deserialize_json_test_array_of_primitives_field() {
    typedef struct {
        int arr1[10];
        int arr1_cnt;
        int *arr2;
        int arr2_cnt;
    } test_struct_t;

    static const char js[] = "{\"arr1\":[1,2,3,4,5,62],\"arr2\":[10,20,32,1,44,5,66]}";
    static const size_t js_size = sizeof(js);
    static const int arr1[] = { 1, 2, 3, 4, 5, 62 };
    static const int arr2[] = { 10, 20, 32, 1, 44, 5, 66 };

    yajp_deserialization_ctx_t ctx;
    yajp_deserialization_result_t dres;
    int ret;
    yajp_deserialization_action_t actions[2] = { 0 };
    test_struct_t test_struct = { 0 };

    yajp_deserialization_ctx_init(actions, ARR_LEN(actions), &ctx);
    dres = yajp_deserialize_json_string(js, js_size, &ctx, &test_struct, NULL);

    test_is_equal(dres.status, YAJP_DESERIALIZATION_RESULT_STATUS_OK, "Deserialization failed");

    test_is_equal(memcmp(test_struct.arr1, arr1, sizeof(arr1)), 0, "Structure wasn't deserialized correctly");
    test_is_equal(test_struct.arr1_cnt, ARR_LEN(arr1), "Structure wasn't deserialized correctly");

    test_is_equal(memcmp(test_struct.arr2, arr2, sizeof(arr2)), 0, "Structure wasn't deserialized correctly");
    test_is_equal(test_struct.arr2_cnt, ARR_LEN(arr2), "Structure wasn't deserialized correctly");

    return TEST_RESULT_PASSED;
}

static test_result_t yajp_deserialize_json_test_array_of_strings_field() {
    return TEST_RESULT_PASSED;
}