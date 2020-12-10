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
#include "parser.h"
#include "deserialization_misc.h"

#include <stdlib.h>
#include <string.h>

typedef struct {
    void *user_data;
    void *result;
    void *parser; // todo: find a way to allocate parser on stack
    yajp_lexer_input_t input;
} yajp_parser_data_t;

static int yajp_try_set_value(const yajp_deserialization_ctx_t *ctx,
                              const yajp_lexer_token_t *field, const yajp_lexer_token_t *value,
                              void *base, void *user_data) {
    uint8_t *address = base;
    int ret = -1;
    const yajp_deserialization_action_t *action;

    action = yajp_get_action(ctx, field->attributes.value, field->attributes.value_size);
    if (NULL != action ) {
        address += action->offset;
        ret = action->setter(field->attributes.value, field->attributes.value_size,
                             value->attributes.value, value->attributes.value_size,
                             address, user_data);
    }

    return ret;
}

static int yajp_deserialize(yajp_parser_data_t *data, const yajp_deserialization_ctx_t *ctx) {
    int ret, i = 0;
    yajp_lexer_token_t tokens[10]; // need to 3 tokens to handle key and value: a : b
    yajp_lexer_token_t *cur_token;
    yajp_parser_recognized_action_t action;

    do {
        cur_token = &tokens[i % 10];

        ret = yajp_lexer_get_next_token(&data->input, cur_token);
        if (ret) {
            return ret;
        }

        action.is_recognized = false;

        yajp_parser_parse(data->parser, cur_token->token, cur_token, &action);

        if (action.is_recognized) {
            ret = yajp_try_set_value(ctx, action.field, action.value, data->result, data->user_data);
            if (ret) {
                return ret;
            }
        }

        i++;

    } while (YAJP_TOKEN_EOF != cur_token->token);

    return 0;
}












int yajp_deserialize_json_string(const char *json, size_t json_size, const yajp_deserialization_ctx_t *ctx,
                                 void *result, void *user_data) {
    FILE *json_stream;
    int ret = -1;

    if (NULL != (json_stream = fmemopen((void *) json, json_size, "r"))) {
        ret = yajp_deserialize_json_stream(json_stream, ctx, result, user_data);
        fclose(json_stream);
    }

    return ret;
}

int yajp_deserialize_json_stream(FILE *json, const yajp_deserialization_ctx_t *ctx, void *result, void *user_data) {
    int ret;
    yajp_parser_data_t data = {
            .parser = NULL,
            .result = result,
            .user_data = user_data
    };

    if (0 == (ret = yajp_lexer_init_input(json, &data.input))) {
#if DEBUG
        yajp_parser_trace(stderr, "yajp_parser --->");
#endif
        data.parser = yajp_parser_allocate(malloc);
        if (NULL != data.parser) {
            ret = yajp_deserialize(&data, ctx);
            yajp_parser_release(data.parser, free);
        }
        yajp_lexer_release_input(&data.input);
    }

    return ret;
}