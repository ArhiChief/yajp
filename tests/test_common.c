/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * main.c
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

#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <time.h>

#include "test_common.h"

static int read_test_number(const char *num_str, long *test_num) {
    char *end;
    long result;

    errno = 0;
    result = strtol(num_str, &end, 10);

    if (errno != 0 || end == num_str) {
        return -1;
    }

    *test_num = result;
    return 0;
}

static const char *test_result_to_string(test_result_t result) {
    switch (result) {
        case TEST_RESULT_PASSED:
            return STRINGIFY(TEST_RESULT_PASSED);
        case TEST_RESULT_FAILED:
            return STRINGIFY(TEST_RESULT_FAILED);
        case TEST_RESULT_INVALID_ARGS:
            return STRINGIFY(TEST_RESULT_INVALID_ARGS);
        case TEST_RESULT_NOT_FOUND:
            return STRINGIFY(TEST_RESULT_NOT_FOUND);
    }

    return "Unknown";
}

static test_result_t run_test(long test_num) {
    const test_case_t *test_case;
    test_result_t result;

    test_case = &test_suite[test_num - 1];

    fprintf(stderr, "| Starting test %ld/%ld:\t%s\n|--> %s\n", test_num, test_count, test_case->name, test_case->description);
    result = test_case->method(test_case->argc, test_case->argv);
    fprintf(stderr, "| Test %ld/%ld ended with %s\n\n", test_num, test_count, test_result_to_string(result));

    return result;
}

int main(int argc, char **argv) {
    long test_num;
    time_t t;
    test_result_t result = TEST_RESULT_PASSED, tmp_res;

    if (argc < 2) {
        return TEST_RESULT_INVALID_ARGS;
    }

    srand((unsigned) time(&t));

    if (0 == strncasecmp(argv[1], "all", sizeof("all"))) {
        for (test_num = 1; test_num <= test_count; ++test_num) {
            if (TEST_RESULT_PASSED == (tmp_res = run_test(test_num))) {
                result = tmp_res;
            }
        }

        return result;
    }

    if (read_test_number(argv[1], &test_num)) {
        return TEST_RESULT_INVALID_ARGS;
    }

    // test number should be in range from 1 to test_count-1
    if ((test_num - 1) < 0 || test_num > test_count) {
        return TEST_RESULT_NOT_FOUND;
    }

    return run_test(test_num);
}