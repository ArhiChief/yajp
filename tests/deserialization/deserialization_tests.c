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
#include "test_common.h"

#include "yajp/deserialization.h"
#include "yajp/deserialization_routine.h"

/* test cases prototypes */
static test_result_t yajp_deserialize_json_string_test1(int argc, char **argv);

/* test suite declaration and initialization */
const test_case_t test_suite[] = {
        REGISTER_TEST_CASE(yajp_deserialize_json_string_test1, 1, yajp_deserialize_json_string, "with primitive json")
};

/* test suite tests count declaration and initialization */
const long test_count = sizeof(test_suite) / sizeof(test_suite[0]);

static test_result_t yajp_deserialize_json_string_test1(int argc, char **argv) {

    typedef struct {
        int my_field;
    } test_struct_t;

    static const char js[] = "{\"my_field\":12345}";
    static const char js_size = sizeof(js);

    yajp_deserialization_ctx_t ctx;
    yajp_deserialization_action_t action;
    test_struct_t test_struct;

    yajp_deserialization_action_init("my_field", sizeof("my_field"), offsetof(test_struct_t, my_field), sizeof(int),
                                     YAJP_DESERIALIZATION_ACTION_TYPE_FIELD, yajp_parse_int, &action);

    yajp_deserialization_ctx_init(&action, 1, &ctx);

    yajp_deserialize_json_string(js, js_size, &ctx, &test_struct, NULL);

    return TEST_RESULT_PASSED;
}