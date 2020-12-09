/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * parser_tests.c
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

#include "parser.h"

/* test cases prototypes */
static test_result_t yajp_parser_parse_test1(int argc, char **argv);

/* test suite declaration and initialization */
const test_case_t test_suite[] = {
        REGISTER_TEST_CASE(yajp_parser_parse_test1, 1, yajp_parser_parse, "with primitive token stream")
};

/* test suite tests count declaration and initialization */
const long test_count = sizeof(test_suite) / sizeof(test_suite[0]);


static test_result_t yajp_parser_parse_test1(int argc, char **argv) {
    static const yajp_token_type_t tokens[] = { YAJP_TOKEN_ABEGIN, YAJP_TOKEN_NUMBER, YAJP_TOKEN_AEND};
    static const int token_len = ARR_LEN(tokens);

    void *parser;
    yajp_parser_recognized_action_t recognized_action = YAJP_PARSER_RECOGNIZED_ACTION_NONE;
    int i;

    parser = yajp_parser_allocate(malloc);

    for (i = 0; i < token_len; i++) {
        yajp_parser_parse(parser, tokens[i], tokens[i], &recognized_action);
    }

    yajp_parser_release(parser, free);

    return TEST_RESULT_PASSED;
}