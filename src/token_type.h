/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * deserialization_common.h
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


#ifndef YAJP_TOKEN_TYPE_H
#define YAJP_TOKEN_TYPE_H

#include "parser_tokens.h"

/**
 * \brief   Represents token types scanned by lexer
 */
typedef enum {
    YAJP_TOKEN_EOF      = 0,                            /* End of file token type */
    YAJP_TOKEN_OBEGIN   = YAJP_PARSER_TOKEN_OBEGIN,     /* Object begin token type ({) */
    YAJP_TOKEN_OEND     = YAJP_PARSER_TOKEN_OEND,       /* Object end token type (}) */
    YAJP_TOKEN_COMMA    = YAJP_PARSER_TOKEN_COMMA,      /* Elements separator token type (,) */
    YAJP_TOKEN_STRING   = YAJP_PARSER_TOKEN_STRING,     /* String token type ("hello world") */
    YAJP_TOKEN_COLON    = YAJP_PARSER_TOKEN_COLON,      /* Pair separator token type (:) */
    YAJP_TOKEN_ABEGIN   = YAJP_PARSER_TOKEN_ABEGIN,     /* Array begin token type ([) */
    YAJP_TOKEN_AEND     = YAJP_PARSER_TOKEN_AEND,       /* Array end token type (]) */
    YAJP_TOKEN_BOOLEAN  = YAJP_PARSER_TOKEN_BOOLEAN,    /* Boolean token type (true || false) */
    YAJP_TOKEN_NUMBER   = YAJP_PARSER_TOKEN_NUMBER,     /* Number token type (1.23e-2) */
    YAJP_TOKEN_NULL     = YAJP_PARSER_TOKEN_NULL        /* Null token type (null) */
} yajp_token_type_t; /* Token types */

#endif //YAJP_TOKEN_TYPE_H
