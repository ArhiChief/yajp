/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * test_common.h
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


#ifndef YAJP_TEST_COMMON_H
#define YAJP_TEST_COMMON_H

#include <stdlib.h>
#include <stdio.h>

/**
 * Possible test case return value
 */
typedef enum {
    TEST_RESULT_PASSED          = 0,    /* Test successfully passed */
    TEST_RESULT_FAILED          = 1,    /* Test failed */
    TEST_RESULT_INVALID_ARGS    = 2,    /* Test can't be run due to invalid passed arguments */
    TEST_RESULT_NOT_FOUND       = 3     /* Test doesn't exits in test suite */
} test_result_t;

/**
 *  Declaration of method what is used to execute test case
 */
typedef test_result_t (*test_method_t)(int argc, char **argv);

/**
 * Defines test case of test suite
 */
typedef struct {
    const char const *name;         /* Short name of the test */
    const char const *description;  /* Test description */
    test_method_t method;           /* Pointer to function what will execute test */

    int argc;                       /* Amount arguments in argv */
    char **argv;                    /* Arguments passed to test method */
} test_case_t;

/**
 * Total amount of tests in test suite. This value should be initialized in test suite file and have value equal to number of elements in test suite
 */
extern const long test_count;
/**
 * Test suite. Should be initialized in test suite file
 */
extern const test_case_t test_suite[];

#define REGISTER_TEST_CASE(test, test_num, tested_func, test_desc) {            \
    .name = FUNC_NAME(tested_func)" test "#test_num,                            \
    .description = "Testing execution of "FUNC_NAME(tested_func)" "test_desc,   \
    .method = test,                                                             \
    .argc = 0,                                                                  \
    .argv = NULL                                                                \
}
/**
 * Universal macro for testing expressions
 *  @param expr[in]     Expression what will be test
 *  @param errmsg[in]   Format string to be printed in case of negative test
 *  @param ...[in]      Arguments handled by errmsg
 *
 *  @return     Nothing returned on positive test. Otherwise, errmsg with variadic parameters printed to stderr and
 *              return TEST_RESULT_FAILED.
 *
 *  @note   expr should be able to be explicitly casted to bool or integer
 */
#define test_expression(expr, errmsg, ...) do {                                                     \
    if (!(expr)) {                                                                                  \
        fprintf(stderr, "|--> ERROR: %s:%d: " errmsg "\n", __FILE__, __LINE__, ##__VA_ARGS__);      \
        return(TEST_RESULT_FAILED);                                                                 \
    }                                                                                               \
} while (0)

/* Some test macros */
/**
 *  Test pointer to be equal to NULL
 *  @param ptr[in]      Pointer to be tested
 *  @param errmsg[in]   Format string to be printed in case of negative test
 *  @param ...[in]      Arguments handled by errmsg
 *
 *  @return     @see test_expression definition
 */
#define test_is_null(ptr, errmsg, ...) do { test_expression((NULL == (ptr)), errmsg, ##__VA_ARGS__); } while(0)

/**
 *  Test pointer to be not equal to NULL
 *  @param ptr[in]      Pointer to be tested
 *  @param errmsg[in]   Format string to be printed in case of negative test
 *  @param ...[in]      Arguments handled by errmsg
 *
 *  @return     @see test_expression definition
 */
#define test_is_not_null(ptr, errmsg, ...) do { test_expression((NULL != (ptr)), errmsg, ##__VA_ARGS__); } while(0)

/**
 *  Test two parameters for equality
 *  @param a[in]        Left argument
 *  @param b[in]        Right argument
 *  @param errmsg[in]   Format string to be printed in case of negative test
 *  @param ...[in]      Arguments handled by errmsg
 *
 *  @return     @see test_expression definition
 */
#define test_is_equal(a, b, errmsg, ...) do { test_expression(((a) == (b)), errmsg, ##__VA_ARGS__); } while(0)

/**
 *  Test two parameters for non equality
 *  @param a[in]        Left argument
 *  @param b[in]        Right argument
 *  @param errmsg[in]   Format string to be printed in case of negative test
 *  @param ...[in]      Arguments handled by errmsg
 *
 *  @return     @see test_expression definition
 */
#define test_is_not_equal(a, b, errmsg, ...) do { test_expression(((a) != (b)), errmsg, ##__VA_ARGS__); } while(0)

/**
 * Test parameter to be false
 *  @param val[in]      Testing value
 *  @param errmsg[in]   Format string to be printed in case of negative test
 *  @param ...[in]      Arguments handled by errmsg
 *
 *  @return     @see test_expression definition
 */
#define test_is_false(val, errmsg, ...) do { test_expression(((val) == (false)), errmsg, ##__VA_ARGS__); } while(0)

/**
 * Test parameter to be true
 *  @param val[in]      Testing value
 *  @param errmsg[in]   Format string to be printed in case of negative test
 *  @param ...[in]      Arguments handled by errmsg
 *
 *  @return     @see test_expression definition
 */
#define test_is_true(val, errmsg, ...) do { test_expression(((val) == (false)), errmsg, ##__VA_ARGS__); } while(0)

/**
 *  Test that left argument is less than right
 *  @param a[in]        Left argument
 *  @param b[in]        Right argument
 *  @param errmsg[in]   Format string to be printed in case of negative test
 *  @param ...[in]      Arguments handled by errmsg
 *
 *  @return     @see test_expression definition
 */
#define test_is_lt(a, b, errmsg, ...) do { test_expression(((a) < (b)), errmsg, ##__VA_ARGS__); } while(0)

/**
 *  Test that left argument is less than or equal to right
 *  @param a[in]        Left argument
 *  @param b[in]        Right argument
 *  @param errmsg[in]   Format string to be printed in case of negative test
 *  @param ...[in]      Arguments handled by errmsg
 *
 *  @return     @see test_expression definition
 */
#define test_is_lte(a, b, errmsg, ...) do { test_expression(((a) <= (b)), errmsg, ##__VA_ARGS__); } while(0)

/**
 *  Test that left argument is greater than right
 *  @param a[in]        Left argument
 *  @param b[in]        Right argument
 *  @param errmsg[in]   Format string to be printed in case of negative test
 *  @param ...[in]      Arguments handled by errmsg
 *
 *  @return     @see test_expression definition
 */
#define test_is_gt(a, b, errmsg, ...) do { test_expression(((a) > (b)), errmsg, ##__VA_ARGS__); } while(0)

/**
 *  Test that left argument is greater than or equal to right
 *  @param a[in]        Left argument
 *  @param b[in]        Right argument
 *  @param errmsg[in]   Format string to be printed in case of negative test
 *  @param ...[in]      Arguments handled by errmsg
 *
 *  @return     @see test_expression definition
 */
#define test_is_gte(a, b, errmsg, ...) do { test_expression(((a) >= (b)), errmsg, ##__VA_ARGS__); } while(0)


#define UNUSED __attribute__ ((unused))

/**
 * Wraps passed token with quotes converting it into constant string
 * @param val[in]   Item to wrap with quotes
 * @return  Constant string with content of characters passed to val
 */
#define STRINGIFY(val) #val

/**
 * Return string with function name
 * @param val[in]   Item to wrap with quotes
 * @return  Constant string with content of characters passed to val
 */
#define FUNC_NAME(val) STRINGIFY(val)"()"

#define str_size_without_null(str) ((ARR_LEN(str) - 1) * sizeof(*(str)))
#define ARR_LEN(a) (sizeof(a))/(sizeof(*a))
#endif //YAJP_TEST_COMMON_H
