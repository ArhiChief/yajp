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
#include <errno.h>
#include <stdio.h>

typedef struct {
    void *user_data;
    void *parser;
    yajp_lexer_input_t *lexer_input;
} yajp_deserialization_data_t;

// function prototypes
static yajp_deserialization_result_status_t yajp_deserialize(yajp_deserialization_data_t *data,
                                                             const yajp_deserialization_ctx_t *ctx,
                                                             void *deserializing_struct);

static yajp_deserialization_result_status_t yajp_deserialize_value(yajp_deserialization_data_t *data,
                                                                   const yajp_deserialization_ctx_t *ctx,
                                                                   const yajp_lexer_token_t *name,
                                                                   void *deserializing_struct);

static yajp_deserialization_result_status_t yajp_skip_json_element(yajp_deserialization_data_t *data);

static yajp_deserialization_result_status_t yajp_parse_value(yajp_deserialization_data_t *data,
                                                             const yajp_lexer_token_t *name, yajp_value_setter_t setter,
                                                             bool allocate, size_t allocation_size, void *field);

static yajp_deserialization_result_status_t yajp_invoke_setter(yajp_value_setter_t setter,
                                                               const uint8_t *name, size_t name_size,
                                                               const uint8_t *value, size_t value_size,
                                                               bool allocate, size_t allocation_size,
                                                               void *user_data, void *field);

yajp_deserialization_result_t yajp_deserialize_json_string(const char *json, size_t json_size,
                                                           const yajp_deserialization_ctx_t *ctx,
                                                           void *deserializing_struct, void *user_data) {
    FILE *json_stream;
    void *parser;
    yajp_lexer_input_t lexer_input;
    yajp_deserialization_result_t deserialization_result;
    yajp_deserialization_data_t deserialization_data;

#if DEBUG
    yajp_parser_trace(stderr, "parser => ");
#endif

    json_stream = fmemopen((char *) json, json_size,
                           "r"); // it's ok to cast from `const char *` to `char *` because stream will be created for readonly

    if (NULL == json_stream) {
        deserialization_result.status = YAJP_DESERIALIZATION_RESULT_STATUS_ERRNO_SET;
        goto end;
    }

    if (yajp_lexer_init_input(json_stream, &lexer_input)) {
        deserialization_result.status = YAJP_DESERIALIZATION_RESULT_STATUS_ERRNO_SET;
        goto release_stream;
    }

    parser = yajp_parser_allocate(malloc);
    if (NULL == parser) {
        deserialization_result.status = YAJP_DESERIALIZATION_RESULT_STATUS_ERRNO_SET;
        goto release_lexer;
    }

    yajp_parser_init(parser);

    deserialization_data.lexer_input = &lexer_input;
    deserialization_data.parser = parser;
    deserialization_data.user_data = user_data;

    deserialization_result.status = yajp_deserialize(&deserialization_data, ctx, deserializing_struct);

#ifdef YAJP_TRACK_STREAM
    if (YAJP_DESERIALIZATION_RESULT_STATUS_OK != deserialization_result.status) {
        deserialization_result.line_num = lexer_input.line_num;
        deserialization_result.column_num = lexer_input.column_num;
    }
#endif

    yajp_parser_finalize(deserialization_data.parser);
    yajp_parser_release(deserialization_data.parser, free);

release_lexer:
    yajp_lexer_release_input(&lexer_input);
release_stream:
    fclose(json_stream);
end:
    return deserialization_result;
}

yajp_deserialization_result_t yajp_deserialize_json_stream(FILE *json, const yajp_deserialization_ctx_t *ctx,
                                                           void *deserializing_struct, void *user_data) {
    void *parser;
    yajp_lexer_input_t lexer_input;
    yajp_deserialization_result_t deserialization_result;
    yajp_deserialization_data_t deserialization_data;

#if DEBUG
    yajp_parser_trace(stderr, "parser => ");
#endif

    if (yajp_lexer_init_input(json, &lexer_input)) {
        deserialization_result.status = YAJP_DESERIALIZATION_RESULT_STATUS_ERRNO_SET;
        goto end;
    }

    parser = yajp_parser_allocate(malloc);
    if (NULL == parser) {
        deserialization_result.status = YAJP_DESERIALIZATION_RESULT_STATUS_ERRNO_SET;
        goto release_lexer;
    }

    yajp_parser_init(parser);

    deserialization_data.lexer_input = &lexer_input;
    deserialization_data.parser = parser;
    deserialization_data.user_data = user_data;

    deserialization_result.status = yajp_deserialize(&deserialization_data, ctx, deserializing_struct);

#ifdef YAJP_TRACK_STREAM
    if (YAJP_DESERIALIZATION_RESULT_STATUS_OK != deserialization_result.status) {
        deserialization_result.line_num = lexer_input.line_num;
        deserialization_result.column_num = lexer_input.column_num;
    }
#endif

    yajp_parser_finalize(parser);
    yajp_parser_release(parser, free);

release_lexer:
    yajp_lexer_release_input(&lexer_input);
end:
    return deserialization_result;
}

static yajp_deserialization_result_status_t yajp_deserialize(yajp_deserialization_data_t *data,
                                                             const yajp_deserialization_ctx_t *ctx,
                                                             void *deserializing_struct) {
    yajp_deserialization_result_status_t result;
    yajp_lexer_token_t picked_token;
    yajp_parser_recognized_action_t recognized = {.token = NULL, .recognized = false};


    if (yajp_lexer_get_next_token(data->lexer_input, &picked_token)) {
        return YAJP_DESERIALIZATION_RESULT_STATUS_UNRECOGNIZED_TOKEN;
    }

    if (YAJP_TOKEN_EOF == picked_token.token) {
        result = YAJP_DESERIALIZATION_RESULT_STATUS_OK;
        yajp_parser_parse(data->parser, picked_token.token, &picked_token, &recognized);
        goto end;
    }

    yajp_parser_parse(data->parser, picked_token.token, &picked_token, &recognized);

    if (recognized.recognized) {
        result = yajp_deserialize_value(data, ctx, recognized.token, deserializing_struct);
        if (YAJP_DESERIALIZATION_RESULT_STATUS_OK != result) {
            goto end;
        }
    }

    result = yajp_deserialize(data, ctx, deserializing_struct);

end:
    yajp_lexer_release_token(&picked_token);
    return result;
}

static yajp_deserialization_result_status_t yajp_deserialize_value(yajp_deserialization_data_t *data,
                                                                   const yajp_deserialization_ctx_t *ctx,
                                                                   const yajp_lexer_token_t *name,
                                                                   void *deserializing_struct) {

    const yajp_deserialization_action_t *action;

    action = yajp_find_action_in_context(ctx, name->attributes.value, name->attributes.value_size);

    // TODO: Add handling of unknown json items
    if (NULL == action) {
        return yajp_skip_json_element(data);
    }

    switch (action->options) {
        case (YAJP_DESERIALIZATION_ACTION_OPTIONS_TYPE_STRING | YAJP_DESERIALIZATION_ACTION_OPTIONS_ALLOCATE):
        case YAJP_DESERIALIZATION_ACTION_OPTIONS_TYPE_STRING:
            return yajp_parse_value(data, name, action->option_params.field_string.setter,
                                    action->option_params.field_string.allocate, sizeof(char), // for '\0' symbol
                                    ((uint8_t *)deserializing_struct) + action->offset);
        case YAJP_DESERIALIZATION_ACTION_OPTIONS_TYPE_PRIMITIVE:
            return yajp_parse_value(data, name, action->option_params.field_primitive.setter, false, 0,
                                    ((uint8_t *)deserializing_struct) + action->offset);
        default:
            return YAJP_DESERIALIZATION_RESULT_STATUS_UNRECOGNIZED_TOKEN;
    }
}

static yajp_deserialization_result_status_t yajp_parse_value(yajp_deserialization_data_t *data,
                                                             const yajp_lexer_token_t *name, yajp_value_setter_t setter,
                                                             bool allocate, size_t allocation_size, void *field) {
    static const int tokens_cnt = 2;
    yajp_deserialization_result_status_t result;
    int i = tokens_cnt;
    yajp_lexer_token_t picked_tokens[tokens_cnt];
    yajp_lexer_token_t *cur_tok;
    yajp_parser_recognized_action_t recognized;

    do {
        cur_tok = &picked_tokens[tokens_cnt - i];
        yajp_lexer_get_next_token(data->lexer_input, cur_tok);
        yajp_parser_parse(data->parser, cur_tok->token, cur_tok, &recognized);
    } while (--i);

    if (recognized.recognized) {
        if (allocate) {
            allocation_size += recognized.token->attributes.value_size;
        }

        result = yajp_invoke_setter(setter, name->attributes.value, name->attributes.value_size,
                                  recognized.token->attributes.value,recognized.token->attributes.value_size,
                                  allocate, allocation_size, data->user_data, field);
    } else {
        result = YAJP_DESERIALIZATION_RESULT_STATUS_EXPECTED_VALUE;
    }

    for (i = 0; i < tokens_cnt; i++) {
        cur_tok = &picked_tokens[i];
        yajp_lexer_release_token(cur_tok);
    }

    return result;
}

static yajp_deserialization_result_status_t yajp_invoke_setter(yajp_value_setter_t setter,
                                                               const uint8_t *name, size_t name_size,
                                                               const uint8_t *value, size_t value_size,
                                                               bool allocate, size_t allocation_size,
                                                               void *user_data, void *field) {
    void *tmp;
    int ret;

    if (allocate && allocation_size > 0) {
        tmp = malloc(allocation_size);
        if (NULL == tmp) {
            return YAJP_DESERIALIZATION_RESULT_STATUS_ERRNO_SET;
        }
        *((void **) (field)) = tmp;
    }

    ret = setter(name, name_size, value, value_size, field, user_data);

    return ret
           ? YAJP_DESERIALIZATION_RESULT_STATUS_DESERIALIZATION_ERROR
           : YAJP_DESERIALIZATION_RESULT_STATUS_OK;
}

static yajp_deserialization_result_status_t yajp_skip_json_element(yajp_deserialization_data_t *data) {
    yajp_lexer_token_t picked_token = { 0 };
    yajp_parser_recognized_action_t recognized_action;
    yajp_token_type_t picked_token_type;
    int open_brackets_count;

    // look forward to see next token and decide strategy;
    if (yajp_lexer_get_next_token(data->lexer_input, &picked_token)) {
        return YAJP_DESERIALIZATION_RESULT_STATUS_UNRECOGNIZED_TOKEN;
    }
    picked_token_type = picked_token.token;

    yajp_parser_parse(data->parser, picked_token.token, &picked_token, &recognized_action);
    yajp_lexer_release_token(&picked_token);

    switch (picked_token_type) {
        case YAJP_TOKEN_EOF:
            return YAJP_DESERIALIZATION_RESULT_STATUS_UNEXPECTED_EOF;
        case YAJP_TOKEN_OBEGIN:
        case YAJP_TOKEN_ABEGIN: { // skipping object or array
            open_brackets_count = 1;

            do {
                if (yajp_lexer_get_next_token(data->lexer_input, &picked_token)) {
                    return YAJP_DESERIALIZATION_RESULT_STATUS_UNRECOGNIZED_TOKEN;
                }

                if ((YAJP_TOKEN_OBEGIN == picked_token.token) || (YAJP_TOKEN_ABEGIN == picked_token.token)) {
                    open_brackets_count++;
                } else if ((YAJP_TOKEN_OEND == picked_token.token) || (YAJP_TOKEN_AEND == picked_token.token)) {
                    open_brackets_count++;
                }

                yajp_parser_parse(data->parser, picked_token.token, &picked_token, &recognized_action);
                yajp_lexer_release_token(&picked_token);
            } while (open_brackets_count != 0);

            return YAJP_DESERIALIZATION_RESULT_STATUS_OK;
        }
        case YAJP_TOKEN_NUMBER:
        case YAJP_TOKEN_STRING:
        case YAJP_TOKEN_BOOLEAN:
        case YAJP_TOKEN_NULL: { // skipping primitive or string value
            do {
                if (yajp_lexer_get_next_token(data->lexer_input, &picked_token)) {
                    return YAJP_DESERIALIZATION_RESULT_STATUS_UNRECOGNIZED_TOKEN;
                }
                picked_token_type = picked_token.token;
                yajp_parser_parse(data->parser, picked_token.token, &picked_token, &recognized_action);
                yajp_lexer_release_token(&picked_token);
            } while ((YAJP_TOKEN_COMMA != picked_token_type) && (YAJP_TOKEN_OEND != picked_token_type));

            return YAJP_DESERIALIZATION_RESULT_STATUS_OK;
        }
        default:
            return YAJP_DESERIALIZATION_RESULT_STATUS_EXPECTED_VALUE;
    }
}