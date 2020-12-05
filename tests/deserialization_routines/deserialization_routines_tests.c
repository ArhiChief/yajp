/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * deserialization_routines_tests.c
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
 
#include "test_common.h"

#include <errno.h>

#include "yajp/deserialization_routine.h"

/* test cases prototypes */
static test_result_t yajp_parse_int_test1(int argc, char **argv);

/* test suite declaration and initialization */
const test_case_t test_suite[] = {
        {
            .method = yajp_parse_int_test1,
            .name = FUNC_NAME(yajp_parse_int)" test 1",
            .description = "Test execution of "FUNC_NAME(yajp_parse_int)" with value '1'",
            .argc = 0,
            .argv = NULL
        }
};
/* test suite tests count declaration and initialization */
const long test_count = sizeof(test_suite) / sizeof(test_suite[0]);

static test_result_t yajp_parse_int_test1(UNUSED int argc, UNUSED char **argv) {
    static const uint8_t num[] = "1";
    static const int expected = 1;
    int result = !expected;
    int ret;

    ret = yajp_parse_int(NULL, 0, &num, str_size_without_null(num), &result, NULL);

    test_is_equal(ret, 0, FUNC_NAME(yajp_parse_int)" returned non 0");
    test_is_equal(errno, 0, FUNC_NAME(yajp_parse_int)" set errno to %d", errno);
    test_is_equal(result, expected, FUNC_NAME(yajp_parse_int)" returned unexpected value. Expected: %d, Result: %d", expected, result);

    return TEST_RESULT_PASSED;
}