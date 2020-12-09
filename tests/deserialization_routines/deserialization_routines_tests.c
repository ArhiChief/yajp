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

#include <limits.h>
#include <stdbool.h>

#include "yajp/deserialization_routine.h"

/* test cases prototypes */
static test_result_t yajp_parse_short_test_null(int argc, char **argv);
static test_result_t yajp_parse_short_test_empty_string(int argc, char **argv);
static test_result_t yajp_parse_short_test_invalid_string(int argc, char **argv);
static test_result_t yajp_parse_short_test_number_with_leading_plus(int argc, char **argv);
static test_result_t yajp_parse_short_test_number_with_leading_minus(int argc, char **argv);
static test_result_t yajp_parse_short_test_number_without_leading_sign(int argc, char **argv);
static test_result_t yajp_parse_short_test_real_number(int argc, char **argv);
static test_result_t yajp_parse_short_test_number_less_than_short_min(int argc, char **argv);
static test_result_t yajp_parse_short_test_number_greater_than_short_max(int argc, char **argv);

static test_result_t yajp_parse_int_test_null(int argc, char **argv);
static test_result_t yajp_parse_int_test_empty_string(int argc, char **argv);
static test_result_t yajp_parse_int_test_invalid_string(int argc, char **argv);
static test_result_t yajp_parse_int_test_number_with_leading_plus(int argc, char **argv);
static test_result_t yajp_parse_int_test_number_with_leading_minus(int argc, char **argv);
static test_result_t yajp_parse_int_test_number_without_leading_sign(int argc, char **argv);
static test_result_t yajp_parse_int_test_real_number(int argc, char **argv);
static test_result_t yajp_parse_int_test_number_less_than_int_min(int argc, char **argv);
static test_result_t yajp_parse_int_test_number_greater_than_int_max(int argc, char **argv);

static test_result_t yajp_parse_long_int_test_null(int argc, char **argv);
static test_result_t yajp_parse_long_int_test_empty_string(int argc, char **argv);
static test_result_t yajp_parse_long_int_test_invalid_string(int argc, char **argv);
static test_result_t yajp_parse_long_int_test_number_with_leading_plus(int argc, char **argv);
static test_result_t yajp_parse_long_int_test_number_with_leading_minus(int argc, char **argv);
static test_result_t yajp_parse_long_int_test_number_without_leading_sign(int argc, char **argv);
static test_result_t yajp_parse_long_int_test_real_number(int argc, char **argv);
static test_result_t yajp_parse_long_int_test_number_less_than_long_int_min(int argc, char **argv);
static test_result_t yajp_parse_long_int_test_number_greater_than_long_int_max(int argc, char **argv);

static test_result_t yajp_parse_long_long_int_test_null(int argc, char **argv);
static test_result_t yajp_parse_long_long_int_test_empty_string(int argc, char **argv);
static test_result_t yajp_parse_long_long_int_test_invalid_string(int argc, char **argv);
static test_result_t yajp_parse_long_long_int_test_number_with_leading_plus(int argc, char **argv);
static test_result_t yajp_parse_long_long_int_test_number_with_leading_minus(int argc, char **argv);
static test_result_t yajp_parse_long_long_int_test_number_without_leading_sign(int argc, char **argv);
static test_result_t yajp_parse_long_long_int_test_real_number(int argc, char **argv);
static test_result_t yajp_parse_long_long_int_test_number_less_than_long_long_int_min(int argc, char **argv);
static test_result_t yajp_parse_long_long_int_test_number_greater_than_long_long_int_max(int argc, char **argv);

static test_result_t yajp_parse_bool_test_null(int argc, char **argv);
static test_result_t yajp_parse_bool_test_empty_string(int argc, char **argv);
static test_result_t yajp_parse_bool_test_true(int argc, char **argv);
static test_result_t yajp_parse_bool_test_false(int argc, char **argv);
static test_result_t yajp_parse_bool_test_invalid_string(int argc, char **argv);
static test_result_t yajp_parse_bool_test_valid_in_content_but_invalid_as_value(int argc, char **argv);

/* test suite declaration and initialization */
const test_case_t test_suite[] = {
        REGISTER_TEST_CASE(yajp_parse_short_test_null, 1, yajp_parse_short, "when value is NULL"),
        REGISTER_TEST_CASE(yajp_parse_short_test_empty_string, 2, yajp_parse_short, "when value is empty string"),
        REGISTER_TEST_CASE(yajp_parse_short_test_invalid_string, 3, yajp_parse_short, "when value is invalid string"),
        REGISTER_TEST_CASE(yajp_parse_short_test_number_with_leading_plus, 4, yajp_parse_short, "when value have leading plus"),
        REGISTER_TEST_CASE(yajp_parse_short_test_number_with_leading_minus, 5, yajp_parse_short, "when value have leading minus"),
        REGISTER_TEST_CASE(yajp_parse_short_test_number_without_leading_sign, 6, yajp_parse_short,"when value plain number"),
        REGISTER_TEST_CASE(yajp_parse_short_test_real_number, 7, yajp_parse_short, "when value real number"),
        REGISTER_TEST_CASE(yajp_parse_short_test_number_less_than_short_min, 8, yajp_parse_short, "when value is less than SHRT_MIN"),
        REGISTER_TEST_CASE(yajp_parse_short_test_number_greater_than_short_max, 9, yajp_parse_short, "when value is greater than SHRT_MAX"),

        REGISTER_TEST_CASE(yajp_parse_int_test_null, 1, yajp_parse_int, "when value is NULL"),
        REGISTER_TEST_CASE(yajp_parse_int_test_empty_string, 2, yajp_parse_int, "when value is empty string"),
        REGISTER_TEST_CASE(yajp_parse_int_test_invalid_string, 3, yajp_parse_int, "when value is invalid string"),
        REGISTER_TEST_CASE(yajp_parse_int_test_number_with_leading_plus, 4, yajp_parse_int, "when value have leading plus"),
        REGISTER_TEST_CASE(yajp_parse_int_test_number_with_leading_minus, 5, yajp_parse_int, "when value have leading minus"),
        REGISTER_TEST_CASE(yajp_parse_int_test_number_without_leading_sign, 6, yajp_parse_int, "when value plain number"),
        REGISTER_TEST_CASE(yajp_parse_int_test_real_number, 7, yajp_parse_int, "when value real number"),
        REGISTER_TEST_CASE(yajp_parse_int_test_number_less_than_int_min, 8, yajp_parse_int, "when value is less than INT_MIN"),
        REGISTER_TEST_CASE(yajp_parse_int_test_number_greater_than_int_max, 9, yajp_parse_int, "when value is greater than INT_MAX"),

        REGISTER_TEST_CASE(yajp_parse_long_int_test_null, 1, yajp_parse_long_int, "when value is NULL"),
        REGISTER_TEST_CASE(yajp_parse_long_int_test_empty_string, 2, yajp_parse_long_int, "when value is empty string"),
        REGISTER_TEST_CASE(yajp_parse_long_int_test_invalid_string, 3, yajp_parse_long_int, "when value is invalid string"),
        REGISTER_TEST_CASE(yajp_parse_long_int_test_number_with_leading_plus, 4, yajp_parse_long_int, "when value have leading plus"),
        REGISTER_TEST_CASE(yajp_parse_long_int_test_number_with_leading_minus, 5, yajp_parse_long_int, "when value have leading minus"),
        REGISTER_TEST_CASE(yajp_parse_long_int_test_number_without_leading_sign, 6, yajp_parse_long_int, "when value plain number"),
        REGISTER_TEST_CASE(yajp_parse_long_int_test_real_number, 7, yajp_parse_long_int, "when value real number"),
        REGISTER_TEST_CASE(yajp_parse_long_int_test_number_less_than_long_int_min, 8, yajp_parse_long_int, "when value is less than LINT_MIN"),
        REGISTER_TEST_CASE(yajp_parse_long_int_test_number_greater_than_long_int_max, 9, yajp_parse_long_int, "when value is greater than LINT_MAX"),

        REGISTER_TEST_CASE(yajp_parse_long_long_int_test_null, 1, yajp_parse_long_long_int, "when value is NULL"),
        REGISTER_TEST_CASE(yajp_parse_long_long_int_test_empty_string, 2, yajp_parse_long_long_int, "when value is empty string"),
        REGISTER_TEST_CASE(yajp_parse_long_long_int_test_invalid_string, 3, yajp_parse_long_long_int, "when value is invalid string"),
        REGISTER_TEST_CASE(yajp_parse_long_long_int_test_number_with_leading_plus, 4, yajp_parse_long_long_int, "when value have leading plus"),
        REGISTER_TEST_CASE(yajp_parse_long_long_int_test_number_with_leading_minus, 5, yajp_parse_long_long_int, "when value have leading minus"),
        REGISTER_TEST_CASE(yajp_parse_long_long_int_test_number_without_leading_sign, 6, yajp_parse_long_long_int, "when value plain number"),
        REGISTER_TEST_CASE(yajp_parse_long_long_int_test_real_number, 7, yajp_parse_long_long_int, "when value real number"),
        REGISTER_TEST_CASE(yajp_parse_long_long_int_test_number_less_than_long_long_int_min, 8, yajp_parse_long_long_int, "when value is less than LLINT_MIN"),
        REGISTER_TEST_CASE(yajp_parse_long_long_int_test_number_greater_than_long_long_int_max, 9, yajp_parse_long_long_int, "when value is greater than LLINT_MAX"),

        REGISTER_TEST_CASE(yajp_parse_bool_test_null, 1, yajp_parse_short, "when value is NULL"),
        REGISTER_TEST_CASE(yajp_parse_bool_test_empty_string, 2, yajp_parse_short, "when value is empty string"),
        REGISTER_TEST_CASE(yajp_parse_bool_test_true, 3, yajp_parse_short, "when value is 'true'"),
        REGISTER_TEST_CASE(yajp_parse_bool_test_false, 4, yajp_parse_short, "when value is 'false'"),
        REGISTER_TEST_CASE(yajp_parse_bool_test_invalid_string, 5, yajp_parse_short, "when value is invalid string"),
        REGISTER_TEST_CASE(yajp_parse_bool_test_valid_in_content_but_invalid_as_value, 6, yajp_parse_short, "when value have valid content but not valid as expected value, i.e. 'tRUe'"),
};

/* test suite tests count declaration and initialization */
const long test_count = sizeof(test_suite) / sizeof(test_suite[0]);


static test_result_t yajp_parse_short_test_null(UNUSED int argc, UNUSED char **argv) {
    static const char *value = NULL;
    static const size_t value_size = 0;
    short result = -1;
    int ret;

    ret = yajp_parse_short(NULL, 0, value, value_size, &result, NULL);

    test_is_not_equal(ret, 0, FUNC_NAME(yajp_parse_short)" returned 0");
    test_is_equal(result, -1, FUNC_NAME(yajp_parse_short)" set 'result'");

    return TEST_RESULT_PASSED;
}

static test_result_t yajp_parse_short_test_empty_string(UNUSED int argc, UNUSED char **argv) {
    static const char value[] = {};
    static const size_t value_size = sizeof(value);
    short result = -1;
    int ret;

    ret = yajp_parse_short(NULL, 0, value, value_size, &result, NULL);

    test_is_not_equal(ret, 0, FUNC_NAME(yajp_parse_short)" returned 0");
    test_is_equal(result, -1, FUNC_NAME(yajp_parse_short)" set 'result'");

    return TEST_RESULT_PASSED;
}

static test_result_t yajp_parse_short_test_invalid_string(UNUSED int argc, UNUSED char **argv) {
    static const char value[] = "a332de";
    static const size_t value_size = str_size_without_null(value);
    short result = -1;
    int ret;

    ret = yajp_parse_short(NULL, 0, value, value_size, &result, NULL);

    test_is_not_equal(ret, 0, FUNC_NAME(yajp_parse_short)" returned 0");
    test_is_equal(result, -1, FUNC_NAME(yajp_parse_short)" set 'result'");

    return TEST_RESULT_PASSED;
}

static test_result_t yajp_parse_short_test_number_with_leading_plus(UNUSED int argc, UNUSED char **argv) {
    static const char value[] = "+12";
    static const size_t value_size = str_size_without_null(value);
    short result = -1;
    int ret;

    ret = yajp_parse_short(NULL, 0, value, value_size, &result, NULL);

    test_is_equal(ret, 0, FUNC_NAME(yajp_parse_short)" returned 0");
    test_is_equal(result, 12, FUNC_NAME(yajp_parse_short)" set 'result'");

    return TEST_RESULT_PASSED;
}

static test_result_t yajp_parse_short_test_number_with_leading_minus(UNUSED int argc, UNUSED char **argv) {
    static const char value[] = "-22";
    static const size_t value_size = str_size_without_null(value);
    short result = -1;
    int ret;

    ret = yajp_parse_short(NULL, 0, value, value_size, &result, NULL);

    test_is_equal(ret, 0, FUNC_NAME(yajp_parse_short)" returned 0");
    test_is_equal(result, -22, FUNC_NAME(yajp_parse_short)" set 'result'");

    return TEST_RESULT_PASSED;
}

static test_result_t yajp_parse_short_test_number_without_leading_sign(UNUSED int argc, UNUSED char **argv) {
    static const char value[] = "14";
    static const size_t value_size = str_size_without_null(value);
    short result = -1;
    int ret;

    ret = yajp_parse_short(NULL, 0, value, value_size, &result, NULL);

    test_is_equal(ret, 0, FUNC_NAME(yajp_parse_short)" returned 0");
    test_is_equal(result, 14, FUNC_NAME(yajp_parse_short)" set 'result'");

    return TEST_RESULT_PASSED;
}

static test_result_t yajp_parse_short_test_real_number(UNUSED int argc, UNUSED  char **argv) {
    static const char value[] = "14.2";
    static const size_t value_size = str_size_without_null(value);
    short result = -1;
    int ret;

    ret = yajp_parse_short(NULL, 0, value, value_size, &result, NULL);

    test_is_not_equal(ret, 0, FUNC_NAME(yajp_parse_short)" returned 0");
    test_is_equal(result, -1, FUNC_NAME(yajp_parse_short)" set 'result'");

    return TEST_RESULT_PASSED;
}

static test_result_t yajp_parse_short_test_number_less_than_short_min(UNUSED int argc, UNUSED  char **argv) {
    static const int val = SHRT_MIN - 20; // according to standard, int should be larger than short
    char str_val[20];
    size_t conv_len = snprintf(str_val, sizeof(str_val), "%d", val);
    short result = -1;
    int ret;

    ret = yajp_parse_short(NULL, 0, str_val, conv_len, &result, NULL);

    test_is_not_equal(ret, 0, FUNC_NAME(yajp_parse_short)" returned 0");
    test_is_equal(result, -1, FUNC_NAME(yajp_parse_short)" set 'result'");

    return TEST_RESULT_PASSED;
}

static test_result_t yajp_parse_short_test_number_greater_than_short_max(UNUSED int argc, UNUSED  char **argv) {
    static const int val = SHRT_MAX + 20; // according to standard, int should be larger than short
    char str_val[20];
    size_t conv_len = snprintf(str_val, sizeof(str_val), "%d", val);
    short result = -1;
    int ret;

    ret = yajp_parse_short(NULL, 0, str_val, conv_len, &result, NULL);

    test_is_not_equal(ret, 0, FUNC_NAME(yajp_parse_short)" returned 0");
    test_is_equal(result, -1, FUNC_NAME(yajp_parse_short)" set 'result'");

    return TEST_RESULT_PASSED;
}

static test_result_t yajp_parse_int_test_null(int argc, char **argv) {
    static const char *value = NULL;
    static const size_t value_size = 0;
    int result = -1;
    int ret;

    ret = yajp_parse_int(NULL, 0, value, value_size, &result, NULL);

    test_is_not_equal(ret, 0, FUNC_NAME(yajp_parse_short)" returned 0");
    test_is_equal(result, -1, FUNC_NAME(yajp_parse_short)" set 'result'");

    return TEST_RESULT_PASSED;
}

static test_result_t yajp_parse_int_test_empty_string(int argc, char **argv) {
    static const char value[] = {};
    static const size_t value_size = sizeof(value);
    int result = -1;
    int ret;

    ret = yajp_parse_int(NULL, 0, value, value_size, &result, NULL);

    test_is_not_equal(ret, 0, FUNC_NAME(yajp_parse_short)" returned 0");
    test_is_equal(result, -1, FUNC_NAME(yajp_parse_short)" set 'result'");

    return TEST_RESULT_PASSED;
}

static test_result_t yajp_parse_int_test_invalid_string(int argc, char **argv) {
    static const char value[] = "a332de";
    static const size_t value_size = str_size_without_null(value);
    int result = -1;
    int ret;

    ret = yajp_parse_int(NULL, 0, value, value_size, &result, NULL);

    test_is_not_equal(ret, 0, FUNC_NAME(yajp_parse_short)" returned 0");
    test_is_equal(result, -1, FUNC_NAME(yajp_parse_short)" set 'result'");

    return TEST_RESULT_PASSED;
}

static test_result_t yajp_parse_int_test_number_with_leading_plus(int argc, char **argv) {
    static const char value[] = "+123";
    static const size_t value_size = str_size_without_null(value);
    int result = -1;
    int ret;

    ret = yajp_parse_int(NULL, 0, value, value_size, &result, NULL);

    test_is_equal(ret, 0, FUNC_NAME(yajp_parse_short)" returned 0");
    test_is_equal(result, 123, FUNC_NAME(yajp_parse_short)" set 'result'");

    return TEST_RESULT_PASSED;
}

static test_result_t yajp_parse_int_test_number_with_leading_minus(int argc, char **argv) {
    static const char value[] = "-212";
    static const size_t value_size = str_size_without_null(value);
    int result = -1;
    int ret;

    ret = yajp_parse_short(NULL, 0, value, value_size, &result, NULL);

    test_is_equal(ret, 0, FUNC_NAME(yajp_parse_short)" returned 0");
    test_is_equal(result, -212, FUNC_NAME(yajp_parse_short)" set 'result'");

    return TEST_RESULT_PASSED;
}

static test_result_t yajp_parse_int_test_number_without_leading_sign(int argc, char **argv) {
    static const char value[] = "143";
    static const size_t value_size = str_size_without_null(value);
    int result = -1;
    int ret;

    ret = yajp_parse_int(NULL, 0, value, value_size, &result, NULL);

    test_is_equal(ret, 0, FUNC_NAME(yajp_parse_short)" returned 0");
    test_is_equal(result, 143, FUNC_NAME(yajp_parse_short)" set 'result'");

    return TEST_RESULT_PASSED;
}

static test_result_t yajp_parse_int_test_real_number(int argc, char **argv) {
    static const char value[] = "14.2e-4";
    static const size_t value_size = str_size_without_null(value);
    int result = -1;
    int ret;

    ret = yajp_parse_short(NULL, 0, value, value_size, &result, NULL);

    test_is_not_equal(ret, 0, FUNC_NAME(yajp_parse_short)" returned 0");
    test_is_equal(result, -1, FUNC_NAME(yajp_parse_short)" set 'result'");

    return TEST_RESULT_PASSED;
}

static test_result_t yajp_parse_int_test_number_less_than_int_min(int argc, char **argv) {
    static const long long int val = (long long int)INT_MIN - 2000;
    char str_val[24];
    size_t conv_len = snprintf(str_val, sizeof(str_val), "%lld", val);
    int result = -1;
    int ret;

    ret = yajp_parse_int(NULL, 0, str_val, conv_len, &result, NULL);

    test_is_not_equal(ret, 0, FUNC_NAME(yajp_parse_short)" returned 0");
    test_is_equal(result, -1, FUNC_NAME(yajp_parse_short)" set 'result'");

    return TEST_RESULT_PASSED;
}

static test_result_t yajp_parse_int_test_number_greater_than_int_max(int argc, char **argv) {
    static const long long int val = (long long int)INT_MAX + 2000;
    char str_val[24];
    size_t conv_len = snprintf(str_val, sizeof(str_val), "%lld", val);
    int result = -1;
    int ret;

    ret = yajp_parse_short(NULL, 0, str_val, conv_len, &result, NULL);

    test_is_not_equal(ret, 0, FUNC_NAME(yajp_parse_short)" returned 0");
    test_is_equal(result, -1, FUNC_NAME(yajp_parse_short)" set 'result'");

    return TEST_RESULT_PASSED;
}

static test_result_t yajp_parse_long_int_test_null(int argc, char **argv) {
    static const char *value = NULL;
    static const size_t value_size = 0;
    long int result = -1;
    int ret;

    ret = yajp_parse_long_int(NULL, 0, value, value_size, &result, NULL);

    test_is_not_equal(ret, 0, FUNC_NAME(yajp_parse_short)" returned 0");
    test_is_equal(result, -1, FUNC_NAME(yajp_parse_short)" set 'result'");

    return TEST_RESULT_PASSED;
}

static test_result_t yajp_parse_long_int_test_empty_string(int argc, char **argv) {
    static const char value[] = {};
    static const size_t value_size = sizeof(value);
    long int result = -1;
    int ret;

    ret = yajp_parse_long_int(NULL, 0, value, value_size, &result, NULL);

    test_is_not_equal(ret, 0, FUNC_NAME(yajp_parse_short)" returned 0");
    test_is_equal(result, -1, FUNC_NAME(yajp_parse_short)" set 'result'");

    return TEST_RESULT_PASSED;
}

static test_result_t yajp_parse_long_int_test_invalid_string(int argc, char **argv) {
    static const char value[] = "a332de";
    static const size_t value_size = str_size_without_null(value);
    long int result = -1;
    int ret;

    ret = yajp_parse_long_int(NULL, 0, value, value_size, &result, NULL);

    test_is_not_equal(ret, 0, FUNC_NAME(yajp_parse_short)" returned 0");
    test_is_equal(result, -1, FUNC_NAME(yajp_parse_short)" set 'result'");

    return TEST_RESULT_PASSED;
}

static test_result_t yajp_parse_long_int_test_number_with_leading_plus(int argc, char **argv) {
    static const char value[] = "+1254";
    static const size_t value_size = str_size_without_null(value);
    long int result = -1;
    int ret;

    ret = yajp_parse_long_int(NULL, 0, value, value_size, &result, NULL);

    test_is_equal(ret, 0, FUNC_NAME(yajp_parse_short)" returned 0");
    test_is_equal(result, 1254, FUNC_NAME(yajp_parse_short)" set 'result'");

    return TEST_RESULT_PASSED;
}

static test_result_t yajp_parse_long_int_test_number_with_leading_minus(int argc, char **argv) {
    static const char value[] = "-2223";
    static const size_t value_size = str_size_without_null(value);
    long int result = -1;
    int ret;

    ret = yajp_parse_long_int(NULL, 0, value, value_size, &result, NULL);

    test_is_equal(ret, 0, FUNC_NAME(yajp_parse_short)" returned 0");
    test_is_equal(result, -2223, FUNC_NAME(yajp_parse_short)" set 'result'");

    return TEST_RESULT_PASSED;
}

static test_result_t yajp_parse_long_int_test_number_without_leading_sign(int argc, char **argv) {
    static const char value[] = "1443";
    static const size_t value_size = str_size_without_null(value);
    long int result = -1;
    int ret;

    ret = yajp_parse_long_int(NULL, 0, value, value_size, &result, NULL);

    test_is_equal(ret, 0, FUNC_NAME(yajp_parse_short)" returned 0");
    test_is_equal(result, 1443, FUNC_NAME(yajp_parse_short)" set 'result'");

    return TEST_RESULT_PASSED;
}

static test_result_t yajp_parse_long_int_test_real_number(int argc, char **argv) {
    static const char value[] = "14.2";
    static const size_t value_size = str_size_without_null(value);
    long int result = -1;
    int ret;

    ret = yajp_parse_long_int(NULL, 0, value, value_size, &result, NULL);

    test_is_not_equal(ret, 0, FUNC_NAME(yajp_parse_short)" returned 0");
    test_is_equal(result, -1, FUNC_NAME(yajp_parse_short)" set 'result'");

    return TEST_RESULT_PASSED;
}

static test_result_t yajp_parse_long_int_test_number_less_than_long_int_min(int argc, char **argv) {
    static const char value[] = "-9223372036854775999";
    static const size_t value_size = str_size_without_null(value);
    short result = -1;
    int ret;

    ret = yajp_parse_long_int(NULL, 0, value, value_size, &result, NULL);

    test_is_not_equal(ret, 0, FUNC_NAME(yajp_parse_short)" returned 0");
    test_is_equal(result, -1, FUNC_NAME(yajp_parse_short)" set 'result'");

    return TEST_RESULT_PASSED;
}

static test_result_t yajp_parse_long_int_test_number_greater_than_long_int_max(int argc, char **argv) {
    static const char value[] = "9223372036854775999";
    static const size_t value_size = str_size_without_null(value);
    short result = -1;
    int ret;

    ret = yajp_parse_long_int(NULL, 0, value, value_size, &result, NULL);

    test_is_not_equal(ret, 0, FUNC_NAME(yajp_parse_short)" returned 0");
    test_is_equal(result, -1, FUNC_NAME(yajp_parse_short)" set 'result'");

    return TEST_RESULT_PASSED;
}

static test_result_t yajp_parse_long_long_int_test_null(int argc, char **argv) {
    static const char *value = NULL;
    static const size_t value_size = 0;
    long long int result = -1;
    int ret;

    ret = yajp_parse_long_long_int(NULL, 0, value, value_size, &result, NULL);

    test_is_not_equal(ret, 0, FUNC_NAME(yajp_parse_short)" returned 0");
    test_is_equal(result, -1, FUNC_NAME(yajp_parse_short)" set 'result'");

    return TEST_RESULT_PASSED;
}

static test_result_t yajp_parse_long_long_int_test_empty_string(int argc, char **argv) {
    static const char value[] = {};
    static const size_t value_size = sizeof(value);
    long long int result = -1;
    int ret;

    ret = yajp_parse_long_long_int(NULL, 0, value, value_size, &result, NULL);

    test_is_not_equal(ret, 0, FUNC_NAME(yajp_parse_short)" returned 0");
    test_is_equal(result, -1, FUNC_NAME(yajp_parse_short)" set 'result'");

    return TEST_RESULT_PASSED;
}

static test_result_t yajp_parse_long_long_int_test_invalid_string(int argc, char **argv) {
    static const char value[] = "a332de";
    static const size_t value_size = str_size_without_null(value);
    long long int result = -1;
    int ret;

    ret = yajp_parse_long_long_int(NULL, 0, value, value_size, &result, NULL);

    test_is_not_equal(ret, 0, FUNC_NAME(yajp_parse_short)" returned 0");
    test_is_equal(result, -1, FUNC_NAME(yajp_parse_short)" set 'result'");

    return TEST_RESULT_PASSED;
}

static test_result_t yajp_parse_long_long_int_test_number_with_leading_plus(int argc, char **argv) {
    static const char value[] = "+12";
    static const size_t value_size = str_size_without_null(value);
    long long int result = -1;
    int ret;

    ret = yajp_parse_long_long_int(NULL, 0, value, value_size, &result, NULL);

    test_is_equal(ret, 0, FUNC_NAME(yajp_parse_short)" returned 0");
    test_is_equal(result, 12, FUNC_NAME(yajp_parse_short)" set 'result'");

    return TEST_RESULT_PASSED;
}

static test_result_t yajp_parse_long_long_int_test_number_with_leading_minus(int argc, char **argv) {
    static const char value[] = "-22";
    static const size_t value_size = str_size_without_null(value);
    long long int result = -1;
    int ret;

    ret = yajp_parse_long_long_int(NULL, 0, value, value_size, &result, NULL);

    test_is_equal(ret, 0, FUNC_NAME(yajp_parse_short)" returned 0");
    test_is_equal(result, -22, FUNC_NAME(yajp_parse_short)" set 'result'");

    return TEST_RESULT_PASSED;
}

static test_result_t yajp_parse_long_long_int_test_number_without_leading_sign(int argc, char **argv) {
    static const char value[] = "145543";
    static const size_t value_size = str_size_without_null(value);
    long long int result = -1;
    int ret;

    ret = yajp_parse_long_long_int(NULL, 0, value, value_size, &result, NULL);

    test_is_equal(ret, 0, FUNC_NAME(yajp_parse_short)" returned 0");
    test_is_equal(result, 145543, FUNC_NAME(yajp_parse_short)" set 'result'");

    return TEST_RESULT_PASSED;
}

static test_result_t yajp_parse_long_long_int_test_real_number(int argc, char **argv) {
    static const char value[] = "14.2";
    static const size_t value_size = str_size_without_null(value);
    long long int result = -1;
    int ret;

    ret = yajp_parse_long_long_int(NULL, 0, value, value_size, &result, NULL);

    test_is_not_equal(ret, 0, FUNC_NAME(yajp_parse_short)" returned 0");
    test_is_equal(result, -1, FUNC_NAME(yajp_parse_short)" set 'result'");

    return TEST_RESULT_PASSED;
}

static test_result_t yajp_parse_long_long_int_test_number_less_than_long_long_int_min(int argc, char **argv) {
    static const char value[] = "-9223372036854775999";
    static const size_t value_size = str_size_without_null(value);
    long long int result = -1;
    int ret;

    ret = yajp_parse_long_long_int(NULL, 0, value, value_size, &result, NULL);

    test_is_not_equal(ret, 0, FUNC_NAME(yajp_parse_short)" returned 0");
    test_is_equal(result, -1, FUNC_NAME(yajp_parse_short)" set 'result'");

    return TEST_RESULT_PASSED;
}

static test_result_t yajp_parse_long_long_int_test_number_greater_than_long_long_int_max(int argc, char **argv) {
    static const char value[] = "9223372036854775999";
    static const size_t value_size = str_size_without_null(value);
    long long int result = -1;
    int ret;

    ret = yajp_parse_long_long_int(NULL, 0, value, value_size, &result, NULL);

    test_is_not_equal(ret, 0, FUNC_NAME(yajp_parse_short)" returned 0");
    test_is_equal(result, -1, FUNC_NAME(yajp_parse_short)" set 'result'");

    return TEST_RESULT_PASSED;
}

static test_result_t yajp_parse_bool_test_null(int argc, char **argv) {
    static const char *value = NULL;
    static const size_t value_size = 0;
    bool result = false;
    int ret;

    ret = yajp_parse_bool(NULL, 0, value, value_size, &result, NULL);

    test_is_not_equal(ret, 0, FUNC_NAME(yajp_parse_short)" returned 0");
    test_is_equal(result, false, FUNC_NAME(yajp_parse_short)" set 'result'");

    return TEST_RESULT_PASSED;
}

static test_result_t yajp_parse_bool_test_empty_string(int argc, char **argv) {
    static const char value[] = {};
    static const size_t value_size = sizeof(value);
    bool result = false;
    int ret;

    ret = yajp_parse_bool(NULL, 0, value, value_size, &result, NULL);

    test_is_not_equal(ret, 0, FUNC_NAME(yajp_parse_short)" returned 0");
    test_is_equal(result, false, FUNC_NAME(yajp_parse_short)" set 'result'");

    return TEST_RESULT_PASSED;
}

static test_result_t yajp_parse_bool_test_true(int argc, char **argv) {
    static const char value[] = "true";
    static const size_t value_size = str_size_without_null(value);
    bool result = false;
    int ret;

    ret = yajp_parse_bool(NULL, 0, value, value_size, &result, NULL);

    test_is_equal(ret, 0, FUNC_NAME(yajp_parse_short)" returned 0");
    test_is_equal(result, true, FUNC_NAME(yajp_parse_short)" set 'result'");

    return TEST_RESULT_PASSED;
}

static test_result_t yajp_parse_bool_test_false(int argc, char **argv) {
    static const char value[] = "false";
    static const size_t value_size = str_size_without_null(value);
    bool result = true;
    int ret;

    ret = yajp_parse_bool(NULL, 0, value, value_size, &result, NULL);

    test_is_equal(ret, 0, FUNC_NAME(yajp_parse_short)" returned 0");
    test_is_equal(result, false, FUNC_NAME(yajp_parse_short)" set 'result'");

    return TEST_RESULT_PASSED;
}

static test_result_t yajp_parse_bool_test_invalid_string(int argc, char **argv) {
    static const char value[] = "a332de";
    static const size_t value_size = str_size_without_null(value);
    bool result = true;
    int ret;

    ret = yajp_parse_bool(NULL, 0, value, value_size, &result, NULL);

    test_is_not_equal(ret, 0, FUNC_NAME(yajp_parse_short)" returned 0");
    test_is_equal(result, true, FUNC_NAME(yajp_parse_short)" set 'result'");

    return TEST_RESULT_PASSED;
}

static test_result_t yajp_parse_bool_test_valid_in_content_but_invalid_as_value(int argc, char **argv) {
    static const char value[] = "tRUe";
    static const size_t value_size = str_size_without_null(value);
    bool result = false;
    int ret;

    ret = yajp_parse_bool(NULL, 0, value, value_size, &result, NULL);

    test_is_not_equal(ret, 0, FUNC_NAME(yajp_parse_short)" returned 0");
    test_is_equal(result, false, FUNC_NAME(yajp_parse_short)" set 'result'");

    return TEST_RESULT_PASSED;
}
