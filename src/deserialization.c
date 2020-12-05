/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * parser.c
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


#include "yajp/deserialization.h"
#include "lexer.h"
#include "token_type.h"

/* prototypes */
static int yajp_deserialize(yajp_lexer_input_t *, const yajp_deserialization_ctx_t *, void *, void *);



int yajp_deserialize_json_string(const char *json, size_t json_size, const yajp_deserialization_ctx_t *ctx, void *result,
                             void *user_data) {
    FILE *json_stream;
    int ret;

    // cast to void* can be safely done because stream is opened only for read
    if (NULL == (json_stream = fmemopen((void *) json, json_size, "r")))
        return -1;

    ret = yajp_deserialize_json_stream(json_stream, ctx, result, user_data);

    fclose(json_stream);

    return ret;
}

int yajp_deserialize_json_stream(FILE *json, const yajp_deserialization_ctx_t *ctx, void *result, void *user_data) {
    yajp_lexer_input_t lexer_input;

    int ret;

    if (ret = yajp_lexer_init_input(json, &lexer_input)) {
        return ret;
    }

    ret = yajp_deserialize(&lexer_input, ctx, result, user_data);

    yajp_lexer_release_input(&lexer_input);
    return ret;
}


static int
yajp_deserialize(yajp_lexer_input_t *input, const yajp_deserialization_ctx_t *ctx, void *res, void *user_data) {
    int ret, i;
    yajp_lexer_token_t token = {};
    yajp_token_type_t token_type;

    do {
        ret = yajp_lexer_get_next_token(input, &token);
        if (ret) {
            return -1;
        }

        token_type = token.token;


        yajp_lexer_release_token(&token);

    } while (YAJP_TOKEN_EOF != token_type);

    return 0;
}

