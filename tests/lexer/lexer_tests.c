/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * lexer_tests.c
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

#include <stdbool.h>
#include <errno.h>
#include <string.h>
#include <time.h>

/* headers with testing functions */
#include "lexer.h"
#include "lexer_misc.h"


/* test cases prototypes */
static test_result_t yajp_lexer_init_input_test_empty_stream(int argc, char **argv);
static test_result_t yajp_lexer_init_input_test_stream_small(int argc, char **argv);
static test_result_t yajp_lexer_init_input_test_stream_big(int argc, char **argv);
static test_result_t yajp_lexer_init_input_test_stream_equal(int argc, char **argv);

static test_result_t yajp_lexer_release_input_test(int argc, char **argv);

static test_result_t test_yajp_lexer_pick_token_primitive_token(int argc, char **argv);
static test_result_t test_yajp_lexer_pick_token_extended_number(int argc, char **argv);
static test_result_t test_yajp_lexer_pick_token_extended_bool(int argc, char **argv);
static test_result_t test_yajp_lexer_pick_token_extended_small_string(int argc, char **argv);
static test_result_t test_yajp_lexer_pick_token_extended_large_string(int argc, char **argv);

static test_result_t test_yajp_lexer_release_token_primitive(int argc, char **argv);
static test_result_t test_yajp_lexer_release_token_extended_small(int argc, char **argv);
static test_result_t test_yajp_lexer_release_token_extended_big(int argc, char **argv);

static test_result_t test_yajp_lexer_fill_input_no_realloc(int argc, char **argv);
static test_result_t test_yajp_lexer_fill_input_need_realloc(int argc, char **argv);
static test_result_t test_yajp_lexer_fill_input_need_huge_realloc(int argc, char **argv);

static test_result_t test_lexer_complex_json(int argc, char **argv);

/* test suite declaration and initialization */
const test_case_t test_suite[] = {
        {
                .method = yajp_lexer_init_input_test_empty_stream,
                .name = FUNC_NAME(yajp_lexer_init_input)" test 1",
                .description = "Test execution of " FUNC_NAME(yajp_lexer_init_input) " when stream is empty",
                .argc = 0,
                .argv = NULL
        },
        {
                .method = yajp_lexer_init_input_test_stream_small,
                .name = FUNC_NAME(yajp_lexer_init_input)" test 2",
                .description = "Test execution of " FUNC_NAME(yajp_lexer_init_input) " when stream is less than YAJP_BUFFER_SIZE",
                .argc = 0,
                .argv = NULL
        },
        {
                .method = yajp_lexer_init_input_test_stream_big,
                .name = FUNC_NAME(yajp_lexer_init_input)" test 3",
                .description = "Test execution of " FUNC_NAME(yajp_lexer_init_input) " when stream is greater than YAJP_BUFFER_SIZE",
                .argc = 0,
                .argv = NULL
        },
        {
                .method = yajp_lexer_init_input_test_stream_equal,
                .name = FUNC_NAME(yajp_lexer_init_input)" test 4",
                .description = "Test execution of " FUNC_NAME(yajp_lexer_init_input) " when stream is equal YAJP_BUFFER_SIZE",
                .argc = 0,
                .argv = NULL
        },
        {
            .method = yajp_lexer_release_input_test,
            .name = FUNC_NAME(yajp_lexer_release_input)" test 1",
            .description = "Test execution of " FUNC_NAME(yajp_lexer_release_input),
            .argc = 0,
            .argv = NULL
        },
        {
                .method = test_yajp_lexer_pick_token_primitive_token,
                .name = FUNC_NAME(yajp_lexer_pick_token)" test 1",
                .description = "Test execution of "FUNC_NAME(yajp_lexer_pick_token)" with primitive tokens",
                .argc = 0,
                .argv = NULL
        },
        {
                .method = test_yajp_lexer_pick_token_extended_number,
                .name = FUNC_NAME(yajp_lexer_pick_token)" test 2",
                .description = "Test execution of " FUNC_NAME(yajp_lexer_pick_token)" with extended number token",
                .argc = 0,
                .argv = NULL
        },
        {
                .method = test_yajp_lexer_pick_token_extended_bool,
                .name = FUNC_NAME(yajp_lexer_pick_token)" test 3",
                .description = "Test execution of " FUNC_NAME(yajp_lexer_pick_token)" with extended boolean token",
                .argc = 0,
                .argv = NULL
        },
        {
                .method = test_yajp_lexer_pick_token_extended_small_string,
                .name = FUNC_NAME(yajp_lexer_pick_token)" test 4",
                .description = "Test execution of " FUNC_NAME(yajp_lexer_pick_token)" with extended string token with value size less than YAJP_BUFFER_SIZE. Size of input buffer should be YAJP_BUFFER_SIZE",
                .argc = 0,
                .argv = NULL
        },
        {
                .method = test_yajp_lexer_pick_token_extended_large_string,
                .name = FUNC_NAME(yajp_lexer_pick_token)" test 5",
                .description = "Test execution of " FUNC_NAME(yajp_lexer_pick_token)" with extended string token with value size greater than YAJP_BUFFER_SIZE should multiplies by YAJP_BUFFER_SIZE",
                .argc = 0,
                .argv = NULL
        },
        {
                .method = test_yajp_lexer_release_token_primitive,
                .name = FUNC_NAME(yajp_lexer_release_token)" test 1",
                .description = "Test execution of " FUNC_NAME(yajp_lexer_release_token)" on releasing primitive tokens",
                .argc = 0,
                .argv = NULL
        },
        {
                .method = test_yajp_lexer_release_token_extended_small,
                .name = FUNC_NAME(yajp_lexer_release_token)" test 2",
                .description = "Test execution of " FUNC_NAME(yajp_lexer_release_token)" on releasing extended tokens tokens what fits inside internal buffer",
                .argc = 0,
                .argv = NULL
        },
        {
                .method = test_yajp_lexer_release_token_extended_big,
                .name = FUNC_NAME(yajp_lexer_release_token)" test 3",
                .description = "Test execution of " FUNC_NAME(yajp_lexer_release_token)" on releasing extended tokens tokens what doesn't fits inside internal buffer",
                .argc = 0,
                .argv = NULL
        },
        {
                .method = test_yajp_lexer_fill_input_no_realloc,
                .name = FUNC_NAME(yajp_lexer_fill_input)" test 1",
                .description = "Test execution of " FUNC_NAME(yajp_lexer_fill_input)" when no reallocation of internal buffer needed",
                .argc = 0,
                .argv = NULL
        },
        {
                .method = test_yajp_lexer_fill_input_need_realloc,
                .name = FUNC_NAME(yajp_lexer_fill_input)" test 2",
                .description = "Test execution of " FUNC_NAME(yajp_lexer_fill_input)" when reallocation of internal buffer needed",
                .argc = 0,
                .argv = NULL
        },
        {
                .method = test_yajp_lexer_fill_input_need_huge_realloc,
                .name = FUNC_NAME(yajp_lexer_fill_input)" test 3",
                .description = "Test execution of " FUNC_NAME(yajp_lexer_fill_input)" when big reallocation of internal buffer needed",
                .argc = 0,
                .argv = NULL
        },
        {
            .method = test_lexer_complex_json,
            .name = "Lexer full test",
            .description = "Test token recognition with real JSON",
            .argc = 0,
            .argv = NULL,
        }
};
/* test suite tests count declaration and initialization */
const long test_count = sizeof(test_suite) / sizeof(test_suite[0]);

static const uint8_t zero_block[YAJP_BUFFER_SIZE] = { 0 };

/* helper methods */
/**
 * Transform passed string int stream and call tests over it to execute some tests over it.
 *
 * @param val[in]       String for transformation
 * @param val_size[in]  Size of string in bytes
 * @param caller[in]    Name of test case where call was executed
 * @return  Pointer to stream created from passed string
 */
static int create_and_test_stream(const char *val, size_t val_size, const char *caller, FILE **f) {
    *f = fmemopen((void *) val, val_size, "r");

    test_is_not_null(*f, "Failed to execute "FUNC_NAME(fmemopen)" in %s: %d", caller, errno);
    test_is_equal(errno, 0, "Call to execute "FUNC_NAME(fmemopen)" in %s: %d", caller, errno);
    test_is_false(feof(*f), "Stream contains errors in %s", caller);
    test_is_equal(ftell(*f), 0, "Stream is not empty in %s", caller);

    return TEST_RESULT_PASSED;
}
/**
 * Creates random character
 * @return Random char
 */
static char get_random_char() {
    static const char alphabet[] = "abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789{}[]()+-=";
    static const int char_cnt = (sizeof(alphabet) / sizeof(*alphabet)) - 1; // ignore '\0' in the end of string

    int char_num = rand() % (char_cnt + 1);

    return alphabet[char_num];
}

/* tests cases implementation */
static test_result_t yajp_lexer_init_input_test_empty_stream(UNUSED int argc, UNUSED char **argv) {
    static const char js[] = {};
    yajp_lexer_input_t input;
    FILE *f;
    int ret;

    ret = create_and_test_stream(js, sizeof(js), FUNC_NAME(yajp_lexer_init_input_test_empty_stream), &f);
    if (TEST_RESULT_PASSED != ret) {
        return ret;
    }

    ret = yajp_lexer_init_input(f, &input);

    test_is_equal(ret, 0, FUNC_NAME(yajp_lexer_init_input)" returned error: %d", errno);
    test_is_not_null(input.json, "input stream is NULL");
    test_is_false(input.eof, "Stream ended");
    test_is_not_null(input.buffer, "Buffer wasn't allocated");
    test_is_equal(input.buffer_size, YAJP_BUFFER_SIZE * sizeof(*js),
                  "Buffer size is invalid. Should be %d on emtpy stream", YAJP_BUFFER_SIZE);

    test_is_not_null(zero_block, "Failed to allocate zero-block of size %zu: %d", input.buffer_size, errno);
    test_is_equal(memcmp(zero_block, input.buffer, input.buffer_size), 0, "Input buffer wasn't cleared");

    test_is_equal(input.cursor, input.buffer, "Cursor is not pointing to beginning of buffer");
    test_is_equal(input.marker, input.buffer, "Marker is not pointing to beginning of buffer");
    test_is_equal(input.token, input.buffer, "Token is not pointing to beginning of buffer");
    test_is_equal(input.limit, input.buffer + YAJP_BUFFER_SIZE / sizeof(*js),
                  "Cursor is not pointing to beginning of buffer");

#ifdef YAJP_TRACK_STREAM
    test_is_equal(input.line_num, 1, "Parsing line number is not initialized");
    test_is_equal(input.column_num, 1, "Parsing collumn number is not initialized");
#endif

    fclose(f);

    return TEST_RESULT_PASSED;
}

static test_result_t yajp_lexer_init_input_test_stream_small(UNUSED int argc, UNUSED char **argv) {
    static char js[YAJP_BUFFER_SIZE / sizeof(char ) / 2 + 1];
    yajp_lexer_input_t input;
    FILE *f;
    int ret, i;

    memset(js, 0, sizeof(js));

    for (i = 0; i < ARR_LEN(js) - 1; i++) {
        js[i] = get_random_char();
    }

    ret = create_and_test_stream(js, sizeof(js), FUNC_NAME(yajp_lexer_init_input_test_stream_small), &f);
    if (TEST_RESULT_PASSED != ret) {
        return ret;
    }

    ret = yajp_lexer_init_input(f, &input);

    test_is_equal(ret, 0, FUNC_NAME(yajp_lexer_init_input)" returned error: %d", errno);
    test_is_not_null(input.json, "input stream is NULL");
    test_is_false(input.eof, "Stream ended");
    test_is_not_null(input.buffer, "Buffer wasn't allocated");
    test_is_equal(input.buffer_size, YAJP_BUFFER_SIZE * sizeof(*js),
                  "Buffer size is invalid. Should be %d on emtpy stream", YAJP_BUFFER_SIZE);

    // Stream length is less than buffer size. This means that buffer will be partially filled
    test_is_equal(memcmp(input.buffer, js, sizeof(js)), 0, "Content of stream doesn't match content of buffer");
    test_is_not_null(zero_block, "Failed to allocate zero-block of size %zu: %d", input.buffer_size, errno);
    test_is_equal(
            memcmp(input.buffer + ARR_LEN(js), zero_block, input.buffer_size - ARR_LEN(js)),
            0, "Content of stream doesn't match content of buffer");

    test_is_equal(input.cursor, input.buffer, "Cursor is not pointing to beginning of buffer");
    test_is_equal(input.marker, input.buffer, "Marker is not pointing to beginning of buffer");
    test_is_equal(input.token, input.buffer, "Token is not pointing to beginning of buffer");
    test_is_equal(input.limit, input.buffer + YAJP_BUFFER_SIZE / sizeof(*js),
                  "Cursor is not pointing to beginning of buffer");

#ifdef YAJP_TRACK_STREAM
    test_is_equal(input.line_num, 1, "Parsing line number is not initialized");
    test_is_equal(input.column_num, 1, "Parsing collumn number is not initialized");
#endif

    fclose(f);

    return TEST_RESULT_PASSED;
}

static test_result_t yajp_lexer_init_input_test_stream_big(UNUSED int argc, UNUSED char **argv) {
    static char js[(YAJP_BUFFER_SIZE / sizeof(char)) * 2 + 1];
    yajp_lexer_input_t input;
    FILE *f;
    int ret, i;

    memset(js, 0, sizeof(js));

    for (i = 0; i < ARR_LEN(js) - 1; i++) {
        js[i] = get_random_char();
    }

    ret = create_and_test_stream(js, sizeof(js), FUNC_NAME(yajp_lexer_init_input_test_stream_big), &f);
    if (TEST_RESULT_PASSED != ret) {
        return ret;
    }

    ret = yajp_lexer_init_input(f, &input);

    test_is_equal(ret, 0, FUNC_NAME(yajp_lexer_init_input)" returned error: %d", errno);
    test_is_not_null(input.json, "input stream is NULL");
    test_is_false(input.eof, "Stream ended");
    test_is_not_null(input.buffer, "Buffer wasn't allocated");
    test_is_equal(input.buffer_size, YAJP_BUFFER_SIZE * sizeof(*js),
                  "Buffer size is invalid. Should be %d on emtpy stream", YAJP_BUFFER_SIZE);

    // Stream length is greater than buffer size. Whole stream will not fit into buffer
    test_is_equal(memcmp(input.buffer, js, input.buffer_size), 0, "Content of stream doesn't match content of buffer");

    test_is_equal(input.cursor, input.buffer, "Cursor is not pointing to beginning of buffer");
    test_is_equal(input.marker, input.buffer, "Marker is not pointing to beginning of buffer");
    test_is_equal(input.token, input.buffer, "Token is not pointing to beginning of buffer");
    test_is_equal(input.limit, input.buffer + YAJP_BUFFER_SIZE / sizeof(*js),
                  "Cursor is not pointing to beginning of buffer");

#ifdef YAJP_TRACK_STREAM
    test_is_equal(input.line_num, 1, "Parsing line number is not initialized");
    test_is_equal(input.column_num, 1, "Parsing collumn number is not initialized");
#endif

    fclose(f);

    return TEST_RESULT_PASSED;
}

static test_result_t yajp_lexer_init_input_test_stream_equal(UNUSED int argc, UNUSED char **argv) {
    static char js[(YAJP_BUFFER_SIZE / sizeof(char))];
    yajp_lexer_input_t input;
    FILE *f;
    int ret, i;

    memset(js, 0, sizeof(js));

    for (i = 0; i < ARR_LEN(js) - 1; i++) {
        js[i] = get_random_char();
    }

    ret = create_and_test_stream(js, sizeof(js), FUNC_NAME(yajp_lexer_init_input_test_stream_equal), &f);
    if (TEST_RESULT_PASSED != ret) {
        return ret;
    }

    ret = yajp_lexer_init_input(f, &input);

    test_is_equal(ret, 0, FUNC_NAME(yajp_lexer_init_input)" returned error: %d", errno);
    test_is_not_null(input.json, "input stream is NULL");
    test_is_false(input.eof, "Stream ended");
    test_is_not_null(input.buffer, "Buffer wasn't allocated");
    test_is_equal(input.buffer_size, YAJP_BUFFER_SIZE * sizeof(*js),
                  "Buffer size is invalid. Should be %d on emtpy stream", YAJP_BUFFER_SIZE);

    // Stream length is equal buffer size. Whole stream will not fit into buffer
    test_is_equal(input.buffer_size, YAJP_BUFFER_SIZE, "Buffer size is not equal to YAJP_BUFFER_SIZE");
    test_is_equal(memcmp(input.buffer, js, input.buffer_size), 0, "Content of stream doesn't match content of buffer");

    test_is_equal(input.cursor, input.buffer, "Cursor is not pointing to beginning of buffer");
    test_is_equal(input.marker, input.buffer, "Marker is not pointing to beginning of buffer");
    test_is_equal(input.token, input.buffer, "Token is not pointing to beginning of buffer");
    test_is_equal(input.limit, input.buffer + YAJP_BUFFER_SIZE / sizeof(*js),
                  "Cursor is not pointing to beginning of buffer");

#ifdef YAJP_TRACK_STREAM
    test_is_equal(input.line_num, 1, "Parsing line number is not initialized");
    test_is_equal(input.column_num, 1, "Parsing collumn number is not initialized");
#endif

    fclose(f);

    return TEST_RESULT_PASSED;
}

static test_result_t yajp_lexer_release_input_test(UNUSED int argc, UNUSED char **argv) {
    static char js[(YAJP_BUFFER_SIZE / sizeof(char))];
    yajp_lexer_input_t input;
    FILE *f;
    int ret, i;

    memset(js, 0, sizeof(js));

    for (i = 0; i < ARR_LEN(js) - 1; i++) {
        js[i] = get_random_char();
    }

    ret = create_and_test_stream(js, sizeof(js), FUNC_NAME(yajp_lexer_release_input_test), &f);
    if (TEST_RESULT_PASSED != ret) {
        return ret;
    }

    ret = yajp_lexer_init_input(f, &input);

    test_is_equal(ret, 0, FUNC_NAME(yajp_lexer_init_input)" returned error: %d", errno);

    ret = yajp_lexer_release_input(&input);

    test_is_equal(ret, 0, FUNC_NAME(yajp_lexer_release_input)" finished with error: %d", errno);
    test_is_equal(errno, 0, FUNC_NAME(yajp_lexer_release_input)" returned 0, but set errno: %d", errno);

    test_is_null(input.json, FUNC_NAME(yajp_lexer_release_input)"release pointer to stream incorrectly");
    test_is_null(input.buffer, FUNC_NAME(yajp_lexer_release_input)"release buffer incorrectly");
    test_is_equal(input.buffer_size, 0, FUNC_NAME(yajp_lexer_release_input)"release buffer size incorrectly");
    test_is_null(input.limit, FUNC_NAME(yajp_lexer_release_input)"release limit incorrectly");
    test_is_null(input.cursor, FUNC_NAME(yajp_lexer_release_input)"release cursor incorrectly");
    test_is_null(input.marker, FUNC_NAME(yajp_lexer_release_input)"release marker incorrectly");
    test_is_null(input.token, FUNC_NAME(yajp_lexer_release_input)"release token incorrectly");

#ifdef YAJP_TRACK_STREAM
    test_is_equal(input.line_num, 0, FUNC_NAME(yajp_lexer_release_input)"release line number incorrectly");
    test_is_equal(input.column_num, 0, FUNC_NAME(yajp_lexer_release_input)"release column number incorrectly");
#endif

    fclose(f);

    return TEST_RESULT_PASSED;
}

static test_result_t test_yajp_lexer_pick_token_primitive_token(UNUSED int argc, UNUSED char **argv) {
    uint8_t js[] = "{[]},:null";
    yajp_lexer_token_t picked_token;
    const size_t zero_block_size = YAJP_BUFFER_SIZE / sizeof(uint8_t);
    yajp_lexer_input_t intput = {
            .buffer = js,
            .buffer_size = sizeof(js),
            .token = js, // point to '{'
            .cursor = js + 1,
    };
    int ret;

    memset(&picked_token, 0, sizeof(picked_token));

    ret = yajp_lexer_pick_token(YAJP_TOKEN_OBEGIN, &intput, &picked_token);
    test_is_equal(ret, 0, FUNC_NAME(yajp_lexer_pick_token)" finished with error");
    test_is_equal(picked_token.token, YAJP_TOKEN_OBEGIN, "Returned token type is wrong");
    test_is_equal(picked_token.attributes.value, picked_token.attributes.internal_buffer, "Token attributes value doesn't point ot buffer");
    test_is_equal(picked_token.attributes.value_size, 0, "Token attributes value size is not 0");
    test_is_equal(memcmp(picked_token.attributes.internal_buffer, zero_block, zero_block_size), 0, "Token attributes value contains data");

    intput.token++; // point to '['
    intput.cursor++;
    ret = yajp_lexer_pick_token(YAJP_TOKEN_ABEGIN, &intput, &picked_token);
    test_is_equal(ret, 0, FUNC_NAME(yajp_lexer_pick_token)" finished with error");
    test_is_equal(picked_token.token, YAJP_TOKEN_ABEGIN, "Returned token type is wrong");
    test_is_equal(picked_token.attributes.value, picked_token.attributes.internal_buffer, "Token attributes value doesn't point ot buffer");
    test_is_equal(picked_token.attributes.value_size, 0, "Token attributes value size is not 0");
    test_is_equal(memcmp(picked_token.attributes.internal_buffer, zero_block, zero_block_size), 0, "Token attributes value contains data");

    intput.token++; // point to ']'
    intput.cursor++;
    ret = yajp_lexer_pick_token(YAJP_TOKEN_AEND, &intput, &picked_token);
    test_is_equal(ret, 0, FUNC_NAME(yajp_lexer_pick_token)" finished with error");
    test_is_equal(picked_token.token, YAJP_TOKEN_AEND, "Returned token type is wrong");
    test_is_equal(picked_token.attributes.value, picked_token.attributes.internal_buffer, "Token attributes value doesn't point ot buffer");
    test_is_equal(picked_token.attributes.value_size, 0, "Token attributes value size is not 0");
    test_is_equal(memcmp(picked_token.attributes.internal_buffer, zero_block, zero_block_size), 0, "Token attributes value contains data");

    intput.token++; // point to '}'
    intput.cursor++;
    ret = yajp_lexer_pick_token(YAJP_TOKEN_OEND, &intput, &picked_token);
    test_is_equal(ret, 0, FUNC_NAME(yajp_lexer_pick_token)" finished with error");
    test_is_equal(picked_token.token, YAJP_TOKEN_OEND, "Returned token type is wrong");
    test_is_equal(picked_token.attributes.value, picked_token.attributes.internal_buffer, "Token attributes value doesn't point ot buffer");
    test_is_equal(picked_token.attributes.value_size, 0, "Token attributes value size is not 0");
    test_is_equal(memcmp(picked_token.attributes.internal_buffer, zero_block, zero_block_size), 0, "Token attributes value contains data");

    intput.token++; // point to ','
    intput.cursor++;
    ret = yajp_lexer_pick_token(YAJP_TOKEN_COMMA, &intput, &picked_token);
    test_is_equal(ret, 0, FUNC_NAME(yajp_lexer_pick_token)" finished with error");
    test_is_equal(picked_token.token, YAJP_TOKEN_COMMA, "Returned token type is wrong");
    test_is_equal(picked_token.attributes.value, picked_token.attributes.internal_buffer, "Token attributes value doesn't point ot buffer");
    test_is_equal(picked_token.attributes.value_size, 0, "Token attributes value size is not 0");
    test_is_equal(memcmp(picked_token.attributes.internal_buffer, zero_block, zero_block_size), 0, "Token attributes value contains data");

    intput.token++; // point to ':'
    intput.cursor++;
    ret = yajp_lexer_pick_token(YAJP_TOKEN_COLON, &intput, &picked_token);
    test_is_equal(ret, 0, FUNC_NAME(yajp_lexer_pick_token)" finished with error");
    test_is_equal(picked_token.token, YAJP_TOKEN_COLON, "Returned token type is wrong");
    test_is_equal(picked_token.attributes.value, picked_token.attributes.internal_buffer, "Token attributes value doesn't point ot buffer");
    test_is_equal(picked_token.attributes.value_size, 0, "Token attributes value size is not 0");
    test_is_equal(memcmp(picked_token.attributes.internal_buffer, zero_block, zero_block_size), 0, "Token attributes value contains data");

    intput.token++; // point to 'null'
    intput.cursor += 3;
    ret = yajp_lexer_pick_token(YAJP_TOKEN_NULL, &intput, &picked_token);
    test_is_equal(ret, 0, FUNC_NAME(yajp_lexer_pick_token)" finished with error");
    test_is_equal(picked_token.token, YAJP_TOKEN_NULL, "Returned token type is wrong");
    test_is_equal(picked_token.attributes.value, picked_token.attributes.internal_buffer, "Token attributes value doesn't point ot buffer");
    test_is_equal(picked_token.attributes.value_size, 0, "Token attributes value size is not 0");
    test_is_equal(memcmp(picked_token.attributes.internal_buffer, zero_block, zero_block_size), 0, "Token attributes value contains data");

    return TEST_RESULT_PASSED;
}

static test_result_t test_yajp_lexer_pick_token_extended_number(UNUSED int argc, UNUSED char **argv) {
    uint8_t js[] = "+13332.122e-23";

    yajp_lexer_input_t intput = {
            .buffer = js,
            .buffer_size = (sizeof(js) - (1 * sizeof(*js))), // skip leading '\0'
            .token = js, // point to '+'
            .cursor = js + (ARR_LEN(js) - 1), // points to char after last digit
    };

    yajp_lexer_token_t picked_token;
    int ret;

    memset(&picked_token, 0, sizeof(picked_token));

    ret = yajp_lexer_pick_token(YAJP_TOKEN_NUMBER, &intput, &picked_token);
    test_is_equal(ret, 0, FUNC_NAME(yajp_lexer_pick_token)" finished with error");
    test_is_equal(picked_token.token, YAJP_TOKEN_NUMBER, "Returned token type is wrong");
    test_is_equal(picked_token.attributes.value_size, intput.buffer_size, "Size of token value is not equal to buffer size");
    test_is_equal(memcmp(picked_token.attributes.value, js, intput.buffer_size), 0, "Picked token value is not correct");


    if (intput.buffer_size > YAJP_BUFFER_SIZE) {
        // in case if number is larger than buffer capacity
        test_is_not_equal(picked_token.attributes.value, picked_token.attributes.internal_buffer, "Token value points to token buffer");
        test_is_equal(memcmp(picked_token.attributes.internal_buffer, zero_block, YAJP_BUFFER_SIZE), 0, "Picked token buffer has data");
    } else {
        test_is_equal(memcmp(picked_token.attributes.internal_buffer, js, intput.buffer_size), 0, "Picked token value is not correct");
        test_is_equal(picked_token.attributes.value, picked_token.attributes.internal_buffer, "Token value doesn't points to token buffer");
    }

    yajp_lexer_release_token(&picked_token);

    return TEST_RESULT_PASSED;
}

static test_result_t test_yajp_lexer_pick_token_extended_bool(UNUSED int argc, UNUSED char **argv) {
    uint8_t js[] = "true";

    yajp_lexer_input_t input = {
            .buffer = js,
            .buffer_size = (sizeof(js) - (1 * sizeof(*js))), // skip leading '\0'
            .token = js, // point to '+'
            .cursor = js + (ARR_LEN(js) - 1), // points to char after last digit
    };

    yajp_lexer_token_t picked_token;
    int ret;

    memset(&picked_token, 0, sizeof(picked_token));

    ret = yajp_lexer_pick_token(YAJP_TOKEN_BOOLEAN, &input, &picked_token);
    test_is_equal(ret, 0, FUNC_NAME(yajp_lexer_pick_token)" finished with error");
    test_is_equal(picked_token.token, YAJP_TOKEN_BOOLEAN, "Returned token type is wrong");
    test_is_equal(picked_token.attributes.value_size, input.buffer_size, "Size of token value is not equal to buffer size");
    test_is_equal(memcmp(picked_token.attributes.value, js, input.buffer_size), 0, "Picked token value is not correct");


    if (input.buffer_size > YAJP_BUFFER_SIZE) {
        // in case if number is larger than buffer capacity
        test_is_not_equal(picked_token.attributes.value, picked_token.attributes.internal_buffer, "Token value points to token buffer");
        test_is_equal(memcmp(picked_token.attributes.internal_buffer, zero_block, YAJP_BUFFER_SIZE), 0, "Picked token buffer has data");
    } else {
        test_is_equal(memcmp(picked_token.attributes.internal_buffer, js, input.buffer_size), 0, "Picked token value is not correct");
        test_is_equal(picked_token.attributes.value, picked_token.attributes.internal_buffer, "Token value doesn't points to token buffer");
    }

    yajp_lexer_release_token(&picked_token);

    return TEST_RESULT_PASSED;
}

static test_result_t test_yajp_lexer_pick_token_extended_small_string(UNUSED int argc, UNUSED char **argv) {
    const size_t js_siz = YAJP_BUFFER_SIZE;
    uint8_t *js = malloc(js_siz);
    yajp_lexer_input_t input = {
            .buffer = js,
            .buffer_size = js_siz,
            .token = js, // points to trailing quote
            .cursor = js + ((js_siz / sizeof(*js)) - 1) // points to \0 char
    };
    yajp_lexer_token_t picked_token;
    int i, ret;

    memset(&picked_token, 0, sizeof(picked_token));

    memset(js, 0, js_siz);
    js[0] = '"';

    for (i = 1; i < ((js_siz / sizeof(*js)) - 2); ++i) {
        js[i] = get_random_char();
    }

    js[i] = '"';

    ret = yajp_lexer_pick_token(YAJP_TOKEN_STRING, &input, &picked_token);
    test_is_equal(ret, 0, FUNC_NAME(yajp_lexer_pick_token)" finished with error");
    test_is_equal(picked_token.token, YAJP_TOKEN_STRING, "Returned token type is wrong");
    // according to specification, yajp_lexer_pick_token() trims leading and trailing quotes. Size of token value should be less than input buffer size
    // and token value should fit into token buffer
    test_is_lt(picked_token.attributes.value_size, input.buffer_size, "Size of token value not lower than buffer size");
    test_is_equal(picked_token.attributes.value_size, js_siz / sizeof(*js) - 3, "Picked token value size is not correct");
    test_is_not_null(picked_token.attributes.value, "Token value is not initialized");
    test_is_equal(picked_token.attributes.value, picked_token.attributes.internal_buffer, "Token value doesn't point to token buffer");
    test_is_equal(memcmp(picked_token.attributes.value, js + 1, picked_token.attributes.value_size), 0, "Incorrect token value");

    free(js);
    return TEST_RESULT_PASSED;
}

static test_result_t test_yajp_lexer_pick_token_extended_large_string(UNUSED int argc, UNUSED char **argv) {
    const size_t js_siz = 2 * YAJP_BUFFER_SIZE;
    uint8_t *js = malloc(js_siz);
    yajp_lexer_input_t input = {
            .buffer = js,
            .buffer_size = js_siz,
            .token = js, // points to trailing quote
            .cursor = js + ((js_siz / sizeof(*js)) - 1) // points to \0 char
    };
    yajp_lexer_token_t picked_token;
    int i, ret;

    memset(&picked_token, 0, sizeof(picked_token));

    memset(js, 0, js_siz);
    js[0] = '"';

    for (i = 1; i < ((js_siz / sizeof(*js)) - 2); ++i) {
        js[i] = get_random_char();
    }

    js[i] = '"';

    ret = yajp_lexer_pick_token(YAJP_TOKEN_STRING, &input, &picked_token);
    test_is_equal(ret, 0, FUNC_NAME(yajp_lexer_pick_token)" finished with error");
    test_is_equal(picked_token.token, YAJP_TOKEN_STRING, "Returned token type is wrong");
    test_is_equal(picked_token.attributes.value_size, input.buffer_size - 3, "Size of token is wrong");
    test_is_equal(picked_token.attributes.value_size, js_siz / sizeof(*js) - 3, "Picked token value size is not correct");
    test_is_not_null(picked_token.attributes.value, "Token value is not initialized");
    test_is_not_equal(picked_token.attributes.value, picked_token.attributes.internal_buffer, "Token value point to token buffer");
    test_is_equal(memcmp(picked_token.attributes.value, js + 1, picked_token.attributes.value_size), 0, "Incorrect token value");
    test_is_equal(memcmp(picked_token.attributes.internal_buffer, zero_block, YAJP_BUFFER_SIZE), 0, "Token internal buffer contains data");

    free(js);
    return TEST_RESULT_PASSED;
}


static test_result_t test_yajp_lexer_release_token_primitive(UNUSED int argc, UNUSED char **argv) {
    uint8_t js[] = "null";
    yajp_lexer_input_t input = {
            .buffer = js,
            .buffer_size = sizeof(js),
            .token = js,
            .cursor = js + (ARR_LEN(js) - 1)
    };
    yajp_lexer_token_t token;
    int ret;

    ret = yajp_lexer_pick_token(YAJP_TOKEN_NULL, &input, &token);
    test_is_equal(ret, 0, FUNC_NAME(yajp_lexer_pick_token)" finished with error");

    ret = yajp_lexer_release_token(&token);
    test_is_equal(ret, 0, FUNC_NAME(yajp_lexer_release_token)" finished with error");

    test_is_equal(token.token, 0, "Token released incorrectly");
    test_is_equal(token.attributes.value_size, 0, "Token released incorrectly");
    test_is_equal(token.attributes.value, NULL, "Token released incorrectly");
    test_is_equal(memcmp(token.attributes.internal_buffer, zero_block, sizeof(zero_block)), 0, "Token released incorrectly");

    return TEST_RESULT_PASSED;
}

static test_result_t test_yajp_lexer_release_token_extended_small(UNUSED int argc, UNUSED char **argv) {
    uint8_t js[] = "1234";
    yajp_lexer_input_t input = {
            .buffer = js,
            .buffer_size = sizeof(js),
            .token = js,
            .cursor = js + (ARR_LEN(js) - 1)
    };
    yajp_lexer_token_t token;
    int ret;

    ret = yajp_lexer_pick_token(YAJP_TOKEN_NUMBER, &input, &token);
    test_is_equal(ret, 0, FUNC_NAME(yajp_lexer_pick_token)" finished with error");

    ret = yajp_lexer_release_token(&token);
    test_is_equal(ret, 0, FUNC_NAME(yajp_lexer_release_token)" finished with error");

    test_is_equal(token.token, 0, "Token released incorrectly");
    test_is_equal(token.attributes.value_size, 0, "Token released incorrectly");
    test_is_equal(token.attributes.value, NULL, "Token released incorrectly");
    test_is_equal(memcmp(token.attributes.internal_buffer, zero_block, sizeof(zero_block)), 0, "Token released incorrectly");

    return TEST_RESULT_PASSED;
}

static test_result_t test_yajp_lexer_release_token_extended_big(UNUSED int argc, UNUSED char **argv) {
    const size_t js_siz = 2 * YAJP_BUFFER_SIZE;
    uint8_t *js = malloc(js_siz);
    yajp_lexer_input_t input = {
            .buffer = js,
            .buffer_size = js_siz,
            .token = js, // points to trailing quote
            .cursor = js + ((js_siz / sizeof(*js)) - 1) // points to \0 char
    };
    yajp_lexer_token_t token;
    int i, ret;

    memset(&token, 0, sizeof(token));

    memset(js, 0, js_siz);
    js[0] = '"';

    for (i = 1; i < ((js_siz / sizeof(*js)) - 2); ++i) {
        js[i] = get_random_char();
    }

    js[i] = '"';

    ret = yajp_lexer_pick_token(YAJP_TOKEN_STRING, &input, &token);

    test_is_equal(ret, 0, FUNC_NAME(yajp_lexer_pick_token)" finished with error");

    ret = yajp_lexer_release_token(&token);
    test_is_equal(ret, 0, FUNC_NAME(yajp_lexer_release_token)" finished with error");

    test_is_equal(token.token, 0, "Token released incorrectly");
    test_is_equal(token.attributes.value_size, 0, "Token released incorrectly");
    test_is_equal(token.attributes.value, NULL, "Token released incorrectly");
    test_is_equal(memcmp(token.attributes.internal_buffer, zero_block, sizeof(zero_block)), 0, "Token released incorrectly");

    return TEST_RESULT_PASSED;
}

static test_result_t test_yajp_lexer_fill_input_no_realloc(UNUSED int argc, UNUSED char **argv) {
    // todo: implement
    return TEST_RESULT_PASSED;
}

static test_result_t test_yajp_lexer_fill_input_need_realloc(UNUSED int argc, UNUSED char **argv) {
    // todo: implement
    return TEST_RESULT_PASSED;
}

static test_result_t test_yajp_lexer_fill_input_need_huge_realloc(UNUSED int argc, UNUSED char **argv) {
    // todo: implement
    return TEST_RESULT_PASSED;
}

typedef struct {
    const yajp_token_type_t token;
    const char *token_value;
    size_t token_size;

#ifdef YAJP_TRACK_STREAM
    int line_num;
    int column_num;
#endif
} token_checker_t;

static const char json[] = "{\"web-app\": {\n"
                           "  \"servlet\": [   \n"
                           "    {\n"
                           "      \"servlet-name\": \"cofaxCDS\",\n"
                           "      \"servlet-class\": \"org.cofax.cds.CDSServlet\",\n"
                           "      \"init-param\": {\n"
                           "        \"configGlossary:installationAt\": \"Philadelphia, PA\",\n"
                           "        \"configGlossary:adminEmail\"    :               \"ksm@pobox.com\",\n"
                           "        \"configGlossary:poweredBy\":               \"Cofax\",\n"
                           "        \"configGlossary:poweredByIcon\": \"/images/cofax.gif\",\n"
                           "        \"configGlossary:staticPath\": \"/content/static\",\n"
                           "        \"templateProcessorClass\": \"org.cofax.WysiwygTemplate\",\n"
                           "        \"templateLoaderClass\": \"org.cofax.FilesTemplateLoader\",\n"
                           "        \"templatePath\": \"templates\",\n"
                           "        \"templateOverridePath\": \"\",\n"
                           "        \"defaultListTemplate\": \"listTemplate.htm\",\n"
                           "        \"defaultFileTemplate\": \"articleTemplate.htm\",\n"
                           "        \"useJSP\": false,\n"
                           "        \"jspListTemplate\": \"listTemplate.jsp\",\n"
                           "        \"jspFileTemplate\": \"articleTemplate.jsp\",\n"
                           "        \"cachePackageTagsTrack\": 200,\n"
                           "        \"cachePackageTagsStore\": 200,\n"
                           "        \"cachePackageTagsRefresh\": 60,\n"
                           "        \"cacheTemplatesTrack\": 100,\n"
                           "        \"cacheTemplatesStore\": 50,\n"
                           "        \"cacheTemplatesRefresh\": 15,\n"
                           "        \"cachePagesTrack\": 200,\n"
                           "        \"cachePagesStore\": 100,\n"
                           "        \"cachePagesRefresh\": 10,\n"
                           "                      \"cachePagesDirtyRead\": 10,\n"
                           "        \"searchEngineListTemplate\": \"forSearchEnginesList.htm\",\n"
                           "        \"searchEngineFileTemplate\": \"forSearchEngines.htm\",\n"
                           "        \"searchEngineRobotsDb\": \"WEB-INF/robots.db\",\n"
                           "        \"useDataStore\": true,\n"
                           "        \"dataStoreClass\": \"org.cofax.SqlDataStore\",\n"
                           "        \"redirectionClass\": \"org.cofax.SqlRedirection\",\n"
                           "        \"dataStoreName\": \"cofax\",\n"
                           "        \"dataStoreDriver\": \"com.microsoft.jdbc.sqlserver.SQLServerDriver\",\n"
                           "        \"dataStoreUrl\": \"jdbc:microsoft:sqlserver://LOCALHOST:1433;DatabaseName=goon\",\n"
                           "        \"dataStoreUser\": \"sa\",\n"
                           "        \"dataStorePassword\": \"dataStoreTestQuery\",\n"
                           "        \"dataStoreTestQuery\": \"SET NOCOUNT ON;select test='test';\",\n"
                           "        \"dataStoreLogFile\": \"/usr/local/tomcat/logs/datastore.log\",\n"
                           "        \"dataStoreInitConns\": 10,\n"
                           "        \"dataStoreMaxConns\": 100,\n"
                           "        \"dataStoreConnUsageLimit\": 100,\n"
                           "        \"dataStoreLogLevel\": \"debug\",\n"
                           "        \"maxUrlLength\": 500}},\n"
                           "    {\n"
                           "      \"servlet-name\": \"cofaxEmail\",\n"
                           "      \"servlet-class\": \"org.cofax.cds.EmailServlet\",\n"
                           "      \"init-param\": {\n"
                           "      \"mailHost\": \"mail1\",\n"
                           "      \"mailHostOverride\": \"mail2\"}},\n"
                           "    {\n"
                           "      \"servlet-name\": \"cofaxAdmin\",\n"
                           "      \"servlet-class\": \"org.cofax.cds.AdminServlet\"},\n"
                           " \n"
                           "    {\n"
                           "      \"servlet-name\": \"fileServlet\",\n"
                           "      \"servlet-class\": \"org.cofax.cds.FileServlet\"},\n"
                           "    {\n"
                           "      \"servlet-name\": \"cofaxTools\",\n"
                           "      \"servlet-class\": \"org.cofax.cms.CofaxToolsServlet\",\n"
                           "      \"init-param\": {\n"
                           "        \"templatePath\": \"toolstemplates/\",\n"
                           "        \"log\": 1,\n"
                           "        \"logLocation\": \"/usr/local/tomcat/logs/CofaxTools.log\",\n"
                           "        \"logMaxSize\": \"\",\n"
                           "        \"dataLog\": 1,\n"
                           "        \"dataLogLocation\": \"/usr/local/tomcat/logs/dataLog.log\",\n"
                           "        \"dataLogMaxSize\": \"\",\n"
                           "        \"removePageCache\": \"/content/admin/remove?cache=pages&id=\",\n"
                           "        \"removeTemplateCache\": \"/content/admin/remove?cache=templates&id=\",\n"
                           "        \"fileTransferFolder\": \"/usr/local/tomcat/webapps/content/fileTransferFolder\",\n"
                           "        \"lookInContext\": 1,\n"
                           "        \"adminGroupID\": 4,\n"
                           "        \"betaServer\": true}}],\n"
                           "  \"servlet-mapping\": {\n"
                           "    \"cofaxCDS\": \"/\",\n"
                           "    \"cofaxEmail\": \"/cofaxutil/aemail/*\",\n"
                           "    \"cofaxAdmin\": \"/admin/*\",\n"
                           "    \"fileServlet\": \"/static/*\",\n"
                           "    \"cofaxTools\":\t \"/tools/*\"},\n"
                           " \n"
                           "  \"taglib\": {\n"
                           "    \"utf_8_2bytes\" : \"какие-то слова\",\n"
                           "    \"taglib-uri\": \"cofax.tld\",\n"
                           "    \"taglib-location\": \"/WEB-INF/tlds/cofax.tld\"}}}";

#define str_size_without_null(str) ((ARR_LEN(str) - 1) * sizeof(*(str)))

#define token_obegin() { .token = YAJP_TOKEN_OBEGIN, .token_size = 0, }
#define token_oend() { .token = YAJP_TOKEN_OEND, .token_size = 0, }
#define token_string(val) { .token = YAJP_TOKEN_STRING, .token_size = str_size_without_null(val), .token_value = (val) }
#define token_collon() { .token = YAJP_TOKEN_COLON, .token_size = 0 }
#define token_comma() { .token = YAJP_TOKEN_COMMA, .token_size = 0 }
#define token_abegin() { .token = YAJP_TOKEN_ABEGIN, .token_size = 0 }
#define token_aend() { .token = YAJP_TOKEN_AEND, .token_size = 0 }
#define token_number(val) { .token = YAJP_TOKEN_NUMBER, .token_size = str_size_without_null(val), .token_value = (val) }
#define token_bool(val) { .token = YAJP_TOKEN_BOOLEAN, .token_size = str_size_without_null(val), .token_value = (val) }
#define token_eof() { .token = YAJP_TOKEN_EOF, .token_size = 0, .token_value = NULL }

static const size_t json_size = sizeof(json);
static const token_checker_t tokens_checkers[] = {
        token_obegin(), token_string("web-app"), token_collon(), token_obegin(),
        token_string("servlet"), token_collon(), token_abegin(),
        token_obegin(),
        token_string("servlet-name"), token_collon(), token_string("cofaxCDS"), token_comma(),
        token_string("servlet-class"), token_collon(), token_string("org.cofax.cds.CDSServlet"), token_comma(),
        token_string("init-param"), token_collon(), token_obegin(),
        token_string("configGlossary:installationAt"), token_collon(), token_string("Philadelphia, PA"), token_comma(),
        token_string("configGlossary:adminEmail"), token_collon(), token_string("ksm@pobox.com"), token_comma(),
        token_string("configGlossary:poweredBy"), token_collon(), token_string("Cofax"), token_comma(),
        token_string("configGlossary:poweredByIcon"), token_collon(), token_string("/images/cofax.gif"), token_comma(),
        token_string("configGlossary:staticPath"), token_collon(), token_string("/content/static"), token_comma(),
        token_string("templateProcessorClass"), token_collon(), token_string("org.cofax.WysiwygTemplate"), token_comma(),
        token_string("templateLoaderClass"), token_collon(), token_string("org.cofax.FilesTemplateLoader"), token_comma(),
        token_string("templatePath"), token_collon(), token_string("templates"), token_comma(),
        token_string("templateOverridePath"), token_collon(), token_string(""), token_comma(),
        token_string("defaultListTemplate"), token_collon(), token_string("listTemplate.htm"), token_comma(),
        token_string("defaultFileTemplate"), token_collon(), token_string("articleTemplate.htm"), token_comma(),
        token_string("useJSP"), token_collon(), token_bool("false"), token_comma(),
        token_string("jspListTemplate"), token_collon(), token_string("listTemplate.jsp"), token_comma(),
        token_string("jspFileTemplate"), token_collon(), token_string("articleTemplate.jsp"), token_comma(),
        token_string("cachePackageTagsTrack"), token_collon(), token_number("200"), token_comma(),
        token_string("cachePackageTagsStore"), token_collon(), token_number("200"), token_comma(),
        token_string("cachePackageTagsRefresh"), token_collon(), token_number("60"), token_comma(),
        token_string("cacheTemplatesTrack"), token_collon(), token_number("100"), token_comma(),
        token_string("cacheTemplatesStore"), token_collon(), token_number("50"), token_comma(),
        token_string("cacheTemplatesRefresh"), token_collon(), token_number("15"), token_comma(),
        token_string("cachePagesTrack"), token_collon(), token_number("200"), token_comma(),
        token_string("cachePagesStore"), token_collon(), token_number("100"), token_comma(),
        token_string("cachePagesRefresh"), token_collon(), token_number("10"), token_comma(),
        token_string("cachePagesDirtyRead"), token_collon(), token_number("10"), token_comma(),
        token_string("searchEngineListTemplate"), token_collon(), token_string("forSearchEnginesList.htm"), token_comma(),
        token_string("searchEngineFileTemplate"), token_collon(), token_string("forSearchEngines.htm"), token_comma(),
        token_string("searchEngineRobotsDb"), token_collon(), token_string("WEB-INF/robots.db"), token_comma(),
        token_string("useDataStore"), token_collon(), token_bool("true"), token_comma(),
        token_string("dataStoreClass"), token_collon(), token_string("org.cofax.SqlDataStore"), token_comma(),
        token_string("redirectionClass"), token_collon(), token_string("org.cofax.SqlRedirection"), token_comma(),
        token_string("dataStoreName"), token_collon(), token_string("cofax"), token_comma(),
        token_string("dataStoreDriver"), token_collon(), token_string("com.microsoft.jdbc.sqlserver.SQLServerDriver"), token_comma(),
        token_string("dataStoreUrl"), token_collon(), token_string("jdbc:microsoft:sqlserver://LOCALHOST:1433;DatabaseName=goon"), token_comma(),
        token_string("dataStoreUser"), token_collon(), token_string("sa"), token_comma(),
        token_string("dataStorePassword"), token_collon(), token_string("dataStoreTestQuery"), token_comma(),
        token_string("dataStoreTestQuery"), token_collon(), token_string("SET NOCOUNT ON;select test='test';"), token_comma(),
        token_string("dataStoreLogFile"), token_collon(), token_string("/usr/local/tomcat/logs/datastore.log"), token_comma(),
        token_string("dataStoreInitConns"), token_collon(), token_number("10"), token_comma(),
        token_string("dataStoreMaxConns"), token_collon(), token_number("100"), token_comma(),
        token_string("dataStoreConnUsageLimit"), token_collon(), token_number("100"), token_comma(),
        token_string("dataStoreLogLevel"), token_collon(), token_string("debug"), token_comma(),
        token_string("maxUrlLength"), token_collon(), token_number("500"), token_oend(), token_oend(), token_comma(),
        token_obegin(),
        token_string("servlet-name"), token_collon(), token_string("cofaxEmail"), token_comma(),
        token_string("servlet-class"), token_collon(), token_string("org.cofax.cds.EmailServlet"), token_comma(),
        token_string("init-param"), token_collon(), token_obegin(),
        token_string("mailHost"), token_collon(), token_string("mail1"), token_comma(),
        token_string("mailHostOverride"), token_collon(), token_string("mail2"), token_oend(), token_oend(), token_comma(),
        token_obegin(),
        token_string("servlet-name"), token_collon(), token_string("cofaxAdmin"), token_comma(),
        token_string("servlet-class"), token_collon(), token_string("org.cofax.cds.AdminServlet"), token_oend(), token_comma(),
        token_obegin(),
        token_string("servlet-name"), token_collon(), token_string("fileServlet"), token_comma(),
        token_string("servlet-class"), token_collon(), token_string("org.cofax.cds.FileServlet"), token_oend(), token_comma(),
        token_obegin(),
        token_string("servlet-name"), token_collon(), token_string("cofaxTools"), token_comma(),
        token_string("servlet-class"), token_collon(), token_string("org.cofax.cms.CofaxToolsServlet"), token_comma(),
        token_string("init-param"), token_collon(), token_obegin(),
        token_string("templatePath"), token_collon(), token_string("toolstemplates/"), token_comma(),
        token_string("log"), token_collon(), token_number("1"), token_comma(),
        token_string("logLocation"), token_collon(), token_string("/usr/local/tomcat/logs/CofaxTools.log"), token_comma(),
        token_string("logMaxSize"), token_collon(), token_string(""), token_comma(),
        token_string("dataLog"), token_collon(), token_number("1"), token_comma(),
        token_string("dataLogLocation"), token_collon(), token_string("/usr/local/tomcat/logs/dataLog.log"), token_comma(),
        token_string("dataLogMaxSize"), token_collon(), token_string(""), token_comma(),
        token_string("removePageCache"), token_collon(), token_string("/content/admin/remove?cache=pages&id="), token_comma(),
        token_string("removeTemplateCache"), token_collon(), token_string("/content/admin/remove?cache=templates&id="), token_comma(),
        token_string("fileTransferFolder"), token_collon(), token_string("/usr/local/tomcat/webapps/content/fileTransferFolder"), token_comma(),
        token_string("lookInContext"), token_collon(), token_number("1"), token_comma(),
        token_string("adminGroupID"), token_collon(), token_number("4"), token_comma(),
        token_string("betaServer"), token_collon(), token_bool("true"), token_oend(), token_oend(), token_aend(),  token_comma(),
        token_string("servlet-mapping"), token_collon(), token_obegin(),
        token_string("cofaxCDS"), token_collon(), token_string("/"), token_comma(),
        token_string("cofaxEmail"), token_collon(), token_string("/cofaxutil/aemail/*"), token_comma(),
        token_string("cofaxAdmin"), token_collon(), token_string("/admin/*"), token_comma(),
        token_string("fileServlet"), token_collon(), token_string("/static/*"), token_comma(),
        token_string("cofaxTools"), token_collon(), token_string("/tools/*"), token_oend(), token_comma(),

        token_string("taglib"), token_collon(), token_obegin(),
        token_string("utf_8_2bytes"), token_collon(), token_string("какие-то слова"), token_comma(),
        token_string("taglib-uri"), token_collon(), token_string("cofax.tld"), token_comma(),
        token_string("taglib-location"), token_collon(), token_string("/WEB-INF/tlds/cofax.tld"), token_oend(), token_oend(), token_oend(),
        token_eof()
};
static const size_t a = sizeof(tokens_checkers);
static const int tokens_checkers_count = ARR_LEN(tokens_checkers);

static test_result_t test_lexer_complex_json(UNUSED int argc, UNUSED char **argv) {


    FILE *stream;
    yajp_lexer_input_t input;
    yajp_lexer_token_t token;
    const token_checker_t *checker;
    int ret, i;

    ret = create_and_test_stream(json, json_size, FUNC_NAME(test_lexer_complex_json), &stream);
    if (TEST_RESULT_PASSED != ret) {
        return ret;
    }

    ret = yajp_lexer_init_input(stream, &input);
    test_is_equal(ret, 0, FUNC_NAME(yajp_lexer_init_input)" returned non 0 result");
    test_is_equal(errno, 0, FUNC_NAME(yajp_lexer_init_input)" set errno to %d", errno);

    memset(&token, 0, sizeof(token));

    for (i = 0; i < tokens_checkers_count; ++i) {
        checker = &tokens_checkers[i];

        ret = yajp_lexer_get_next_token(&input, &token);
        test_is_equal(ret, 0, FUNC_NAME(yajp_lexer_get_next_token)" returned non 0 result");
        test_is_equal(errno, 0, FUNC_NAME(yajp_lexer_get_next_token)" set errno to %d", errno);

        test_is_equal(token.token, checker->token, "Token type mismatch. Current: %d; Expected: %d", token.token, checker->token);
        test_is_equal(token.attributes.value_size, checker->token_size, "Token size mismatch. Current: %zu; Expected: %zu", token.attributes.value_size, checker->token_size);

        if (yajp_lexer_is_token_primitive(token.token)) {
            test_is_equal(token.attributes.value, token.attributes.internal_buffer, "Token value don't points to token buffer");
            test_is_equal(memcmp(zero_block, token.attributes.value, sizeof(zero_block)), 0, "Token buffer contains data");
        } else {
            test_is_not_null(token.attributes.value, "Token value was not initialized");
            if (checker->token_size <= YAJP_BUFFER_SIZE) {
                test_is_equal(token.attributes.value, token.attributes.internal_buffer, "Token value should points to token buffer because token size <= YAJP_BUFFER_SIZE");
            } else {
                test_is_not_equal(token.attributes.value, token.attributes.internal_buffer, "Token value should not points to token buffer");
            }
            test_is_equal(memcmp(token.attributes.value, checker->token_value, checker->token_size), 0, "Token value was picked incorrect: Current: %s Expected %s;", token.attributes.value, checker->token_value);
        }

#ifdef YAJP_TRACK_STREAM
#endif

        yajp_lexer_release_token(&token);
    }

    return TEST_RESULT_PASSED;
}