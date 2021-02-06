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
#include <stdio.h>

typedef struct yajp_deserialization_data {
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

static yajp_deserialization_result_status_t yajp_skip_json_object(yajp_deserialization_data_t *data);

static yajp_deserialization_result_status_t yajp_parse_primitive_value(yajp_deserialization_data_t *data,
                                                                       const yajp_lexer_token_t *name,
                                                                       const yajp_deserialization_action_t *action,
                                                                       size_t additional_size,
                                                                       void *base_address);

static yajp_deserialization_result_status_t yajp_parse_array_value(yajp_deserialization_data_t *data,
                                                                       const yajp_lexer_token_t *name,
                                                                       const yajp_deserialization_action_t *action,
                                                                       size_t array_item_additional_size,
                                                                       void *base_address);


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

    // it's ok to cast from `const char *` to `char *` because stream will be created for readonly
    json_stream = fmemopen((char *) json, json_size, "r");

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
#define TOKENS_CNT 3
    yajp_deserialization_result_status_t result = YAJP_DESERIALIZATION_RESULT_STATUS_OK;
    yajp_lexer_token_t tokens[TOKENS_CNT];
    yajp_lexer_token_t *current_token;
    yajp_token_type_t last_token;
    yajp_parser_recognized_entity_t recognized_entity;
    int i = 0, ret;

    memset(tokens, 0, sizeof(tokens));

    do {
        current_token = &tokens[i % TOKENS_CNT];
        ret = yajp_lexer_get_next_token(data->lexer_input, current_token);
        if (ret) {
            result = YAJP_DESERIALIZATION_RESULT_STATUS_ERRNO_SET;
            goto end;
        }
        last_token = current_token->token;

        recognized_entity.type = YAJP_PARSER_RECOGNIZED_ENTITY_TYPE_NONE;
        yajp_parser_parse(data->parser, last_token, current_token, &recognized_entity);

        if (YAJP_PARSER_RECOGNIZED_ENTITY_TYPE_KEY == recognized_entity.type) {
            result = yajp_deserialize_value(data, ctx, recognized_entity.token, deserializing_struct);
            if (YAJP_DESERIALIZATION_RESULT_STATUS_OK != result) {
                goto end;
            }
        }

        i++;
        if (i >= TOKENS_CNT) {
            yajp_lexer_release_token(&tokens[i % TOKENS_CNT]);
        }

    } while (YAJP_TOKEN_EOF != last_token);

end:
    for (i = 0; i < TOKENS_CNT; i++) {
        yajp_lexer_release_token(&tokens[i  % TOKENS_CNT]);
    }

    return result;

#undef TOKENS_CNT
}

static yajp_deserialization_result_status_t yajp_deserialize_value(yajp_deserialization_data_t *data,
                                                                   const yajp_deserialization_ctx_t *ctx,
                                                                   const yajp_lexer_token_t *name,
                                                                   void *deserializing_struct) {

    const yajp_deserialization_action_t *action;
    yajp_deserialization_result_status_t status;

    action = yajp_find_action_in_context(ctx, name->attributes.value, name->attributes.value_size);

    // TODO: Add handling of unknown json items
    if (NULL != action) {
        switch (action->options & 0x0FF) {
            case YAJP_DESERIALIZATION_ACTION_OPTIONS_TYPE_PRIMITIVE:
                status = yajp_parse_primitive_value(data, name, action, 0, deserializing_struct);
                break;
            case YAJP_DESERIALIZATION_ACTION_OPTIONS_TYPE_STRING:
                status = yajp_parse_primitive_value(data, name, action, sizeof(char), deserializing_struct);
                break;
            case (YAJP_DESERIALIZATION_ACTION_OPTIONS_TYPE_ARRAY_OF | YAJP_DESERIALIZATION_ACTION_OPTIONS_TYPE_PRIMITIVE):
                status = yajp_parse_array_value(data, name, action, 0, deserializing_struct);
                break;
            case (YAJP_DESERIALIZATION_ACTION_OPTIONS_TYPE_ARRAY_OF | YAJP_DESERIALIZATION_ACTION_OPTIONS_TYPE_STRING):
                status = yajp_parse_array_value(data, name, action, sizeof(char), deserializing_struct);
                break;
            case (YAJP_DESERIALIZATION_ACTION_OPTIONS_TYPE_OBJECT):
            case (YAJP_DESERIALIZATION_ACTION_OPTIONS_TYPE_ARRAY_OF | YAJP_DESERIALIZATION_ACTION_OPTIONS_TYPE_OBJECT):
            default:
                status = YAJP_DESERIALIZATION_RESULT_STATUS_UNRECOGNIZED_TOKEN;
                break;
        }
    } else {
        status = yajp_skip_json_object(data);
    }

    return status;
}

static yajp_deserialization_result_status_t  yajp_parse_primitive_value(yajp_deserialization_data_t *data,
                                                                        const yajp_lexer_token_t *name,
                                                                        const yajp_deserialization_action_t *action,
                                                                        size_t additional_size,
                                                                        void *base_address) {
#define TOKENS_CNT 2 // 2 tokens should be enough to handle value and wait until parser recognize pair
    yajp_lexer_token_t tokens[TOKENS_CNT];
    yajp_lexer_token_t *current_token;
    yajp_parser_recognized_entity_t recognized_entity;
    size_t allocation_size;
    int i = 0, setter_result;
    yajp_deserialization_result_status_t result = YAJP_DESERIALIZATION_RESULT_STATUS_OK;

    memset(tokens, 0, sizeof(tokens));

    do {
        current_token = &tokens[i];
        i++;

        yajp_lexer_get_next_token(data->lexer_input, current_token);

        recognized_entity.type = YAJP_PARSER_RECOGNIZED_ENTITY_TYPE_NONE;
        yajp_parser_parse(data->parser, current_token->token, current_token, &recognized_entity);

        if (YAJP_PARSER_RECOGNIZED_ENTITY_TYPE_PAIR == recognized_entity.type) {
            base_address += action->offset;

            if (action->option_params.primitive_field.allocate) {
                allocation_size = recognized_entity.token->attributes.value_size + additional_size;
            }

            if (action->options & YAJP_DESERIALIZATION_ACTION_OPTIONS_ALLOCATE) {
                void *tmp = malloc(allocation_size);
                if (NULL == tmp) {
                    result = YAJP_DESERIALIZATION_RESULT_STATUS_ERRNO_SET;
                    goto end;
                }

                setter_result = action->option_params.primitive_field.setter(
                        name->attributes.value, name->attributes.value_size,
                        recognized_entity.token->attributes.value, recognized_entity.token->attributes.value_size,
                        tmp, data->user_data);

                if (0 != setter_result) {
                    free(tmp);
                    tmp = NULL;
                }

                *(void **)base_address = tmp;

            } else {
                setter_result = action->option_params.primitive_field.setter(
                        name->attributes.value, name->attributes.value_size,
                        recognized_entity.token->attributes.value, recognized_entity.token->attributes.value_size,
                        base_address, data->user_data);
            }

            if (0 != setter_result) {
                result = YAJP_DESERIALIZATION_RESULT_STATUS_DESERIALIZATION_ERROR;
            }
        }

    } while (YAJP_PARSER_RECOGNIZED_ENTITY_TYPE_PAIR != recognized_entity.type);

end:
    for (i = 0; i < TOKENS_CNT; i++) {
        yajp_lexer_release_token(&tokens[i]);
    }
    return result;

#undef TOKENS_CNT
}

static yajp_deserialization_result_status_t yajp_skip_json_object(yajp_deserialization_data_t *data) {
    yajp_lexer_token_t picked_token = { 0 };
    yajp_parser_recognized_entity_t recognized_entity;
    yajp_token_type_t picked_token_type;
    int open_brackets_count;

    // look forward to see next token and decide strategy;
    if (yajp_lexer_get_next_token(data->lexer_input, &picked_token)) {
        return YAJP_DESERIALIZATION_RESULT_STATUS_UNRECOGNIZED_TOKEN;
    }
    picked_token_type = picked_token.token;

    yajp_parser_parse(data->parser, picked_token.token, &picked_token, &recognized_entity);
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
                    open_brackets_count--;
                }

                yajp_parser_parse(data->parser, picked_token.token, &picked_token, &recognized_entity);
                yajp_lexer_release_token(&picked_token);
            } while (open_brackets_count != 0);

            return YAJP_DESERIALIZATION_RESULT_STATUS_OK;
        }
        case YAJP_TOKEN_NUMBER:
        case YAJP_TOKEN_STRING:
        case YAJP_TOKEN_BOOLEAN:
        case YAJP_TOKEN_NULL: { // skipping primitives
            do {
                if (yajp_lexer_get_next_token(data->lexer_input, &picked_token)) {
                    return YAJP_DESERIALIZATION_RESULT_STATUS_UNRECOGNIZED_TOKEN;
                }
                picked_token_type = picked_token.token;
                yajp_parser_parse(data->parser, picked_token.token, &picked_token, &recognized_entity);
                yajp_lexer_release_token(&picked_token);
            } while ((YAJP_TOKEN_COMMA != picked_token_type) && (YAJP_TOKEN_OEND != picked_token_type));

            return YAJP_DESERIALIZATION_RESULT_STATUS_OK;
        }
        default:
            return YAJP_DESERIALIZATION_RESULT_STATUS_EXPECTED_VALUE;
    }
}

static yajp_deserialization_result_status_t yajp_parse_array_value(yajp_deserialization_data_t *data,
                                                                   const yajp_lexer_token_t *name,
                                                                   const yajp_deserialization_action_t *action,
                                                                   size_t array_item_additional_size,
                                                                   void *base_address) {
    return YAJP_DESERIALIZATION_RESULT_STATUS_OK;
}