/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * lexer_misc.c
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

#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <assert.h>

#include "lexer.h"
#include "lexer_misc.h"

/* helper function prototypes */
static ssize_t yajp_lexer_extend_buffer(yajp_lexer_input_t *input, size_t need);

static int yajp_lexer_read_buffer(FILE *js, uint8_t *buffer, size_t need);

int yajp_lexer_fill_input(yajp_lexer_input_t *input, size_t need) {
    size_t free, shift, allocated;
    uint8_t *buf_ptr;

    if (input->eof) {
        return -1;
    }

    // shift buffer left if possible. can happened if buffer contains recognized tokens in the beginning
    free = input->token - input->buffer;
    buf_ptr = input->limit - free;

    if (0 < free) {
        shift = input->limit - input->token;
        memmove(input->buffer, input->token, shift);
        input->token -= free;
        input->cursor -= free;
        input->marker -= free;
    }

    if (free < need) {
        allocated = yajp_lexer_extend_buffer(input, need);
        if (allocated <= 0) {
            return -1;
        }
        buf_ptr = input->cursor;
        free = allocated;
    }

    return yajp_lexer_read_buffer(input->json, buf_ptr, free);
}

int yajp_lexer_init_input(FILE *js, yajp_lexer_input_t *input) {
    ssize_t allocated;

    input->json = js;
    input->eof = (0 != feof(js));
    input->buffer = NULL;
    input->buffer_size = 0;

#ifdef YAJP_TRACK_STREAM
    input->column_num = 1;
    input->line_num = 1;
#endif

    allocated = yajp_lexer_extend_buffer(input, YAJP_BUFFER_SIZE);
    if (allocated <= 0) {
        return -1;
    }

    input->limit = input->buffer + (allocated / sizeof(*input->buffer));
    input->cursor = input->buffer;
    input->marker = input->buffer;
    input->token = input->buffer;

    if (yajp_lexer_read_buffer(js, input->buffer, allocated)) {
        free(input->buffer);
        return -1;
    }

    return 0;
}

int yajp_lexer_pick_token(yajp_token_type_t tok_type, const yajp_lexer_input_t *input, yajp_lexer_token_t *tok) {
    size_t tok_size;
    uint8_t *tmp = NULL;
    uint8_t *tok_start = input->token;
    uint8_t *tok_end = input->cursor;

    tok->token = tok_type;
    tok->attributes.value = tok->attributes.internal_buffer;

    // no need to store something else because their values can be defined from type
    switch (tok_type) {
        case YAJP_TOKEN_STRING: { // skip \"
            tok_start++;
            tok_end--;
        }
        case YAJP_TOKEN_BOOLEAN:
        case YAJP_TOKEN_NUMBER: {
            tok_size = tok_end - tok_start;
            if (tok_size > YAJP_BUFFER_SIZE) {
                tmp = malloc(tok_size);
                if (NULL == tmp) {
                    return -1;
                }
                tok->attributes.value = tmp;
            }

            memmove(tok->attributes.value, tok_start, tok_size);
            tok->attributes.value_size = tok_size;
        }
            break;
    }

    return 0;
}

int yajp_lexer_release_input(yajp_lexer_input_t *input) {
    free(input->buffer);
    memset(input, 0, sizeof(*input));
    return 0;
}

bool yajp_lexer_is_token_primitive(yajp_token_type_t token) {
    switch (token) {
        case YAJP_TOKEN_EOF:
        case YAJP_TOKEN_OBEGIN:
        case YAJP_TOKEN_OEND:
        case YAJP_TOKEN_COLON:
        case YAJP_TOKEN_COMMA:
        case YAJP_TOKEN_ABEGIN:
        case YAJP_TOKEN_AEND:
        case YAJP_TOKEN_NULL:
            return true;
        case YAJP_TOKEN_STRING:
        case YAJP_TOKEN_BOOLEAN:
        case YAJP_TOKEN_NUMBER:
            return false;
    }

    return false;
}

int yajp_lexer_release_token(yajp_lexer_token_t *token) {
    if (token->attributes.value != token->attributes.internal_buffer) {
        free(token->attributes.value);
    }

    memset(token, 0, sizeof(*token));
    return 0;
}

/**
 * Helper function. Used to extend input buffer for amount of bytes passed by 'need`
 *
 * @param input[in, out]    Lexer buffer which internal buffer have to be extended
 * @param need[in]          Amount of buffer extension in bytes
 *
 * @return  Result amount of bytes on what buffer was extended or -1 in case of error.
 *
 * @note    This function will calculate amount of free place in buffer and if it's less than 'need'. Size of buffer
 *          extension should multiples by YAJP_BUFFER_SIZE.
 *          This function calls realloc(). According to documentation if partition of memory pointed by realloc's first
 *          parameter will overlay other memory partition function will allocate new partition and move content to it.
 *          This actually means that pointers used by lexer to work with buffer can become invalid. In case if this
 *          happened function will initialize this pointers according to new location of buffer.
 */
static ssize_t yajp_lexer_extend_buffer(yajp_lexer_input_t *input, size_t need) {
    uint8_t *tmp;
    const size_t size = input->buffer_size;
    size_t new_size = size;

    do {
        // calculate new size of buffer
        new_size += YAJP_BUFFER_SIZE;
    } while (size + need > new_size);

    tmp = realloc(input->buffer, new_size);
    if (NULL == tmp) {
        return -1;
    }

    if (tmp != input->buffer) {
        // fix pointers because realloc changed address of new buffer
        input->token = tmp + (input->token - input->buffer) / sizeof(*input->token);
        input->marker = tmp + (input->marker - input->buffer) / sizeof(*input->marker);
        input->limit = tmp + new_size / sizeof(*input->limit) / sizeof(*input->limit);
        input->cursor = tmp + (input->cursor - input->buffer) / sizeof(*input->cursor);
        input->buffer = tmp;
    }

    input->buffer_size = new_size;
    return new_size - size;
}

/**
 * Reads requested amount of bytes from stream into buffer
 *
 * @param js[in]            Stream to be read
 * @param buffer[in,out]    Buffer to be filled
 * @param need[in]          Amount of bytes what will be read from stream
 *
 * @return  Result of reading data into buffer. 0 - on success
 *
 * @note    This function handles possible interrupts of read() and will recall read() to read remaining amount of data
 */
static int yajp_lexer_read_buffer(FILE *js, uint8_t *buffer, size_t need) {
    size_t bytes_read;

    while (need != 0 && (bytes_read = fread(buffer, 1, need, js))) {
        if (bytes_read == -1) { /* Check out what happened */
            if (errno == EINTR) {
                errno = 0;
                continue;
            }

            if (ferror(js))
                return -1;
        }

        need -= bytes_read;
        buffer += bytes_read;
    }

    return 0;
}