/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * lexer_misc.h
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


#ifndef YAJP_LEXER_MISC_H
#define YAJP_LEXER_MISC_H

#include "lexer.h"

/**
 * Fills lexer input buffers with data from stream
 *
 * Helper function for yajp_lexer_get_next_token() and function for yajp_lexer_get_next_token().
 *
 * @param input[in, out]    Lexer input what requred to be filled
 * @param need[in]          Amount of bytes need to be read from stream
 * @return  Result of buffer filling. 0 - on success
 *
 * @note This function is internally called by yajp_lexer_get_next_token() function for yajp_lexer_get_next_token() and,
 *       as mentioned in description and name, used to fill input buffer with next partition of data from stream in amount
 *       equal to 'need' bytes.
 *
 *       Internally, this function can shift content of buffer to remove obsolete content and increase amount of free
 *       space.
 *
 *       In case if amount of free space in buffer is not enough to store 'need' amount of bytes buffer will be
 *       extended to the size that will be enough to read requested amount of data. Size of buffer extension will
 *       multiples YAJP_BUFFER_SIZE.
 *
 *       Allocated buffer size will be stored between calls till there is enough free space. This means that if after
 *       shift of content inside buffer, amount of free space will be greater than 'need' buffer will not be shrunk.
 *       This technique is used to reduce amount of calls to realloc().
 */
int yajp_lexer_fill_input(yajp_lexer_input_t *input, size_t need);

/**
 * Picks recognized token from stream.
 *
 * Helper function for yajp_lexer_get_next_token().
 *
 * @param tok_type[in]  Type of token recognized by lexer
 * @param input[in]     Lexer input
 * @param tok[out]      Picked token with type and value
 * @return  Result of picking token from stream. 0 - on success
 *
 * @note    This function is internally called be yajp_lexer_get_next_token() and used as helper function to return
 *          recognized token to caller of yajp_lexer_get_next_token(). If recognized token is STRING, NUMBER of BOOLEAN
 *          (non-primitive token), string value of recognized token will be stored inside yajp_lexer_token_t.attributes
 *          field.
 *
 *          If string value of recognized token doesn't fits into bounds of yajp_lexer_token_t.attributes.internal_buffer,
 *          required amount of memory will be allocated for it and yajp_lexer_token_t.attributes.value will point on it.
 *          Otherwise, yajp_lexer_token_t.attributes.value should point to first element of
 *          yajp_lexer_token_t.attributes.internal_buffer.
 *
 *          In case if recognized token is primitive yajp_lexer_token_t.attributes will be filled with 0.
 *
 *          Recognized STRING values are stored without leading and trailing quotes.
 */
int yajp_lexer_pick_token(yajp_token_type_t tok_type, const yajp_lexer_input_t *input, yajp_lexer_token_t *tok);

#endif //YAJP_LEXER_MISC_H
