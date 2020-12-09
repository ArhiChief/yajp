/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * lexer.h
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


#ifndef YAJP_LEXER_H
#define YAJP_LEXER_H

#include <stdio.h>
#include <stdint.h>
#include <stdbool.h>

#include "token_type.h"

#if !defined(YAJP_TRACK_STREAM)
#   define YAJP_TRACK_STREAM
#endif

/**
 * Size of buffers in bytes
 */
#if !defined(YAJP_BUFFER_SIZE) || YAJP_BUFFER_SIZE < 32
#   define YAJP_BUFFER_SIZE    32
#endif


/**
 *  Represent recognized token picked from stream
 */
typedef struct {
    yajp_token_type_t token;                                        /* Type of picked token */
    struct {
        size_t value_size;                                          /* Size in bytes of picked token value */
        uint8_t *value;                                             /* Pointer to token value. In case if size of value
                                                                     * less than or equal to YAJP_BUFFER_SIZE should
                                                                     * point to internal_buffer
                                                                    */
        uint8_t internal_buffer[YAJP_BUFFER_SIZE / sizeof(uint8_t)];/* Token buffer used to store small values */
    } attributes;                                                   /* Token attributes */
} yajp_lexer_token_t;

/**
 * Represent lexer input
 */
typedef struct {
    FILE *json;         /* Pointer to stream with json */
    uint8_t *buffer;    /* Buffer of scanning chars */
    size_t buffer_size; /* Size of buffer in bytes */

    uint8_t *cursor;    /* The next input character to be read */
    uint8_t *limit;     /* The position after the last available input character */
    uint8_t *marker;    /* The position of the most recent match, if any */
    uint8_t *token;     /* The start of the current lexeme */

    bool eof;           /* End of file reached */

#ifdef YAJP_TRACK_STREAM
    int line_num;       /* Number of reading line */
    int column_num;     /* Number of reading column */
#endif
} yajp_lexer_input_t;

/**
 * Checks if passed token is primitive. In case of primitive token yajp_lexer_token_t.attributes should be fitted with 0
 * @param token[in] Token for test
 * @return  Result of chek. true if token in primitive
 */
bool yajp_lexer_is_token_primitive(yajp_token_type_t token);

/**
 * Initialize lexer input from stream.
 * @param json [in]
 * @param input [out]
 * @return  Returns result of lexer input initialization. 0 - success
 *
 * @note    Function will allocate to internal buffers memory in amount of YAJP_BUFFER_SIZE. It will try also to fill
 *          allocated buffers with content from passed stream.
 */
int yajp_lexer_init_input(FILE *json, yajp_lexer_input_t *input);

/**
 * Release resources initialized by yajp_lexer_init_input().
 * @param input[in]
 * @return  Returns result of lexer input resource realization. 0 - success
 *
 */
int yajp_lexer_release_input(yajp_lexer_input_t *input);

/**
 * Returns next recognized token in stream.
 *
 * @param[in]   input   Lexer input
 * @param[out]  token   Recognized token
 * @return  Result of next recognized token in stream. 0 - on success
 *
 * @note    This is main function used to scan stream. On each call it returns next JSON token.
 */
int yajp_lexer_get_next_token(yajp_lexer_input_t *input, yajp_lexer_token_t *token);

/**
 * Release token content after usage
 * @param[in]   token   Token which resources should be released.
 * @return  Result of token resource releasing. 0 - on success.
 *
 * @note    This function should be called after using of yajp_lexer_token_t
 */
int yajp_lexer_release_token(yajp_lexer_token_t *token);


#endif //YAJP_LEXER_H
