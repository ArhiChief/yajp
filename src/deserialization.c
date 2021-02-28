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
static int yajp_parse(yajp_deserialization_data_t *data, const yajp_deserialization_context_t *ctx, void *address);

static int yajp_deserialize_value(yajp_deserialization_data_t *data,
                                  const yajp_deserialization_context_t *ctx,
                                  const yajp_lexer_token_t *name,
                                  void *deserializing_struct);

static int yajp_skip_json_object(yajp_deserialization_data_t *data);

static int yajp_parse_primitive_value(yajp_deserialization_data_t *data,
                                      const yajp_lexer_token_t *name,
                                      const yajp_deserialization_rule_t *action,
                                      void *address);

static int yajp_parse_array_value(yajp_deserialization_data_t *data,
                                  const yajp_lexer_token_t *name,
                                  const yajp_deserialization_rule_t *action,
                                  void *address);

static int yajp_parse_array_value_internal(yajp_deserialization_data_t *data,
                                           const yajp_lexer_token_t *name,
                                           const yajp_deserialization_rule_t *action,
                                           void *address);

static int yajp_parse_object_value(yajp_deserialization_data_t *data,
                                   const yajp_deserialization_rule_t *action,
                                   void *address);

static int yajp_parse_array_of_objects_value(yajp_deserialization_data_t *data,
                                             const yajp_deserialization_rule_t *action,
                                             void *address);

static int yajp_parse_array_of_objects_value_internal(yajp_deserialization_data_t *data,
                                                      const yajp_deserialization_rule_t *action,
                                                      void *address);




int yajp_deserialize_json_string(const char *json, size_t json_size, const yajp_deserialization_context_t *ctx,
                                 void *address, void *user_data) {
    FILE *json_stream;
    void *parser;
    yajp_lexer_input_t lexer_input;
    int result;
    yajp_deserialization_data_t deserialization_data;

#if DEBUG
    yajp_parser_trace(stderr, "parser => ");
#endif

    // it's ok to cast from `const char *` to `char *` because stream will be created for readonly
    json_stream = fmemopen((char *) json, json_size, "r");

    if (NULL == json_stream) {
        result = -1; // errno set
        goto end;
    }

    if (yajp_lexer_init_input(json_stream, &lexer_input)) {
        result = -1; // errno set
        goto release_stream;
    }

    parser = yajp_parser_allocate(malloc);
    if (NULL == parser) {
        result = -1; // errno set
        goto release_lexer;
    }

    yajp_parser_init(parser);

    deserialization_data.lexer_input = &lexer_input;
    deserialization_data.parser = parser;
    deserialization_data.user_data = user_data;

    result = yajp_parse(&deserialization_data, ctx, address);

#ifdef YAJP_TRACK_STREAM
    if (YAJP_DESERIALIZATION_RESULT_STATUS_OK != deserialization_result.status) {
        deserialization_result.line_num = lexer_input.line_num;
        deserialization_result.column_num = lexer_input.column_num;
    }
#endif

    yajp_parser_release(parser, free);

release_lexer:
    yajp_lexer_release_input(&lexer_input);
release_stream:
    fclose(json_stream);
end:
    return result;
}

int yajp_deserialize_json_stream(FILE *json, const yajp_deserialization_context_t *ctx, void *address, void *user_data) {
    void *parser;
    yajp_lexer_input_t lexer_input;
    int result;
    yajp_deserialization_data_t deserialization_data;

#if DEBUG
    yajp_parser_trace(stderr, "parser => ");
#endif

    if (yajp_lexer_init_input(json, &lexer_input)) {
        result = -1; // errno set
        goto end;
    }

    parser = yajp_parser_allocate(malloc);
    if (NULL == parser) {
        result = -1; // errno set
        goto release_lexer;
    }

    yajp_parser_init(parser);

    deserialization_data.lexer_input = &lexer_input;
    deserialization_data.parser = parser;
    deserialization_data.user_data = user_data;

    result = yajp_parse(&deserialization_data, ctx, address);

#ifdef YAJP_TRACK_STREAM
    if (YAJP_DESERIALIZATION_RESULT_STATUS_OK != deserialization_result.status) {
        deserialization_result.line_num = lexer_input.line_num;
        deserialization_result.column_num = lexer_input.column_num;
    }
#endif

    yajp_parser_release(parser, free);

release_lexer:
    yajp_lexer_release_input(&lexer_input);
end:
    return result;
}

static int yajp_parse(yajp_deserialization_data_t *data, const yajp_deserialization_context_t *ctx, void *address) {
#define TOKENS_CNT 3
    yajp_lexer_token_t tokens[TOKENS_CNT];
    yajp_lexer_token_t *current_token;
    yajp_token_type_t last_token;
    yajp_parser_recognized_entity_t recognized_entity;
    int i = 0, ret, result = 0;

    memset(tokens, 0, sizeof(tokens));

    do {
        current_token = &tokens[i % TOKENS_CNT];
        ret = yajp_lexer_get_next_token(data->lexer_input, current_token);
        if (ret) {
            result = -1; // errno set
            goto end;
        }
        last_token = current_token->token;

        recognized_entity.type = YAJP_PARSER_RECOGNIZED_ENTITY_TYPE_NONE;
        yajp_parser_parse(data->parser, last_token, current_token, &recognized_entity);

        if (YAJP_PARSER_RECOGNIZED_ENTITY_TYPE_KEY == recognized_entity.type) {
            ret = yajp_deserialize_value(data, ctx, recognized_entity.token, address);
            if (ret) {
                result = -1;
                goto end;
            }
        }

        i++;
        if (i >= TOKENS_CNT) {
            current_token = &tokens[i % TOKENS_CNT];
            yajp_lexer_release_token(current_token);
        }

    } while ((YAJP_TOKEN_EOF != last_token) && (YAJP_TOKEN_OEND != last_token));

end:
    for (i = 0; i < TOKENS_CNT; i++) {
        current_token = &tokens[i % TOKENS_CNT];
        yajp_lexer_release_token(current_token);
    }

    return result;

#undef TOKENS_CNT
}

static int yajp_deserialize_value(yajp_deserialization_data_t *data, const yajp_deserialization_context_t *ctx,
                                  const yajp_lexer_token_t *name, void *address) {
    const yajp_deserialization_rule_t *action;
    int result;

    action = yajp_find_action(ctx, name->attributes.value, name->attributes.value_size);

    // TODO: Add handling of unknown json items
    if (NULL != action) {
        address += action->field_offset;

        switch (action->options & 0b00011111) {
            case YAJP_DESERIALIZATION_TYPE_NUMBER:
            case YAJP_DESERIALIZATION_TYPE_STRING:
            case YAJP_DESERIALIZATION_TYPE_BOOLEAN:
                result = yajp_parse_primitive_value(data, name, action, address);
                break;
            case (YAJP_DESERIALIZATION_TYPE_ARRAY_OF | YAJP_DESERIALIZATION_TYPE_NUMBER):
            case (YAJP_DESERIALIZATION_TYPE_ARRAY_OF | YAJP_DESERIALIZATION_TYPE_STRING):
            case (YAJP_DESERIALIZATION_TYPE_ARRAY_OF | YAJP_DESERIALIZATION_TYPE_BOOLEAN):
                result = yajp_parse_array_value(data, name, action, address);
                break;
            case (YAJP_DESERIALIZATION_TYPE_OBJECT):
                result = yajp_parse_object_value(data, action, address);
                break;
            case (YAJP_DESERIALIZATION_TYPE_ARRAY_OF | YAJP_DESERIALIZATION_TYPE_OBJECT):
                result = yajp_parse_array_of_objects_value(data, action, address);
                break;
            default:
                result = -1;
                break;
        }
    } else {
        result = yajp_skip_json_object(data);
    }

    return result;
}

static int yajp_parse_primitive_value(yajp_deserialization_data_t *data, const yajp_lexer_token_t *name,
                                      const yajp_deserialization_rule_t *action, void *address) {
#define TOKENS_CNT 2 // 2 tokens should be enough to handle value and wait until parser recognize pair
    yajp_lexer_token_t tokens[TOKENS_CNT];
    yajp_lexer_token_t *current_token;
    yajp_parser_recognized_entity_t recognized_entity;
    size_t allocation_size;
    int i = 0, setter_result, result = 0;

    memset(tokens, 0, sizeof(tokens));

    do {
        current_token = &tokens[i];
        i++;

        yajp_lexer_get_next_token(data->lexer_input, current_token);

        recognized_entity.type = YAJP_PARSER_RECOGNIZED_ENTITY_TYPE_NONE;
        yajp_parser_parse(data->parser, current_token->token, current_token, &recognized_entity);

        if (YAJP_PARSER_RECOGNIZED_ENTITY_TYPE_PAIR == recognized_entity.type) {

            if (action->allocate) {
                allocation_size = recognized_entity.token->attributes.value_size + action->elem_size;
                void *tmp = malloc(allocation_size);
                if (NULL == tmp) {
                    result = -1;
                    goto end;
                }

                setter_result = action->setter(name->attributes.value, name->attributes.value_size,
                                               recognized_entity.token->attributes.value,
                                               recognized_entity.token->attributes.value_size, tmp, data->user_data);

                if (0 != setter_result) {
                    free(tmp);
                    tmp = NULL;
                }

                *(void **) address = tmp;

            } else {
                setter_result = action->setter(name->attributes.value, name->attributes.value_size,
                        recognized_entity.token->attributes.value, recognized_entity.token->attributes.value_size,
                        address, data->user_data);
            }

            if (0 != setter_result) {
                result = -1;
                goto end;
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

static int yajp_skip_json_object(yajp_deserialization_data_t *data) {
    yajp_lexer_token_t picked_token = {0};
    yajp_parser_recognized_entity_t recognized_entity;
    yajp_token_type_t picked_token_type;
    int open_brackets_count;

    // look forward to see next token and decide strategy;
    if (yajp_lexer_get_next_token(data->lexer_input, &picked_token)) {
        return -1; // unrecognized token
    }
    picked_token_type = picked_token.token;

    yajp_parser_parse(data->parser, picked_token.token, &picked_token, &recognized_entity);
    yajp_lexer_release_token(&picked_token);

    switch (picked_token_type) {
        case YAJP_TOKEN_EOF:
            return -1; // unexpected eof
        case YAJP_TOKEN_OBEGIN:
        case YAJP_TOKEN_ABEGIN: { // skipping object or array
            open_brackets_count = 1;
            do {
                if (yajp_lexer_get_next_token(data->lexer_input, &picked_token)) {
                    return -1; // unrecognized token
                }

                if ((YAJP_TOKEN_OBEGIN == picked_token.token) || (YAJP_TOKEN_ABEGIN == picked_token.token)) {
                    open_brackets_count++;
                } else if ((YAJP_TOKEN_OEND == picked_token.token) || (YAJP_TOKEN_AEND == picked_token.token)) {
                    open_brackets_count--;
                }

                yajp_parser_parse(data->parser, picked_token.token, &picked_token, &recognized_entity);
                yajp_lexer_release_token(&picked_token);
            } while (open_brackets_count != 0);

            return 0;
        }
        case YAJP_TOKEN_NUMBER:
        case YAJP_TOKEN_STRING:
        case YAJP_TOKEN_BOOLEAN:
        case YAJP_TOKEN_NULL: { // skipping primitives
            do {
                if (yajp_lexer_get_next_token(data->lexer_input, &picked_token)) {
                    return -1; // unrecognized token
                }
                picked_token_type = picked_token.token;
                yajp_parser_parse(data->parser, picked_token.token, &picked_token, &recognized_entity);
                yajp_lexer_release_token(&picked_token);
            } while ((YAJP_TOKEN_COMMA != picked_token_type) && (YAJP_TOKEN_OEND != picked_token_type));

            return 0;
        }
        default:
            return -1; // value expected
    }
}

static int yajp_parse_array_value(yajp_deserialization_data_t *data, const yajp_lexer_token_t *name,
                                  const yajp_deserialization_rule_t *action, void *address) {
    yajp_lexer_token_t current_token;
    yajp_parser_recognized_entity_t recognized_entity;
    int result;

    yajp_lexer_get_next_token(data->lexer_input, &current_token);
    if (YAJP_TOKEN_ABEGIN != current_token.token) {
        result = -1; // expected [ token
        goto end;
    }

    yajp_parser_parse(data->parser, current_token.token, &current_token, &recognized_entity);

    if (action->allocate) {
        void *tmp = malloc(action->field_size);
        if (NULL == tmp) {
            return -1; // errno set
        }
        memset(tmp, 0, action->field_size);
        result = yajp_parse_array_value_internal(data, name, action, tmp);

        if (0 != result) {
            free(tmp);
            goto end;
        } else {
            *(void **) address = tmp;
        }
    } else {
        memset(address, 0, action->field_size);
        result = yajp_parse_array_value_internal(data, name, action, address);
    }

    end:
    yajp_lexer_release_token(&current_token);
    return result;
}

static int yajp_parse_array_value_internal(yajp_deserialization_data_t *data, const yajp_lexer_token_t *name,
                                           const yajp_deserialization_rule_t *action, void *address) {
#define TOKEN_CNT 3
    yajp_lexer_token_t tokens[TOKEN_CNT];
    yajp_lexer_token_t *current_token;
    yajp_token_type_t picked_token;

    yajp_parser_recognized_entity_t recognized_entity;
    int i = 0, setter_result, result = 0;
    size_t row_shift = 0;
    void *elem_address;

    size_t *count = address + action->counter_offset;
    bool *final_dim = address + action->final_dym_offset;

    *final_dim = true;

    memset(tokens, 0, sizeof(tokens));

    do {
        current_token = &tokens[i % TOKEN_CNT];
        i++;

        yajp_lexer_get_next_token(data->lexer_input, current_token);
        picked_token = current_token->token;

        recognized_entity.type = YAJP_PARSER_RECOGNIZED_ENTITY_TYPE_NONE;
        yajp_parser_parse(data->parser, picked_token, current_token, &recognized_entity);

        if (YAJP_TOKEN_ABEGIN == picked_token) {
            *final_dim = false;

            elem_address = *(void **) (address + action->rows_offset);
            elem_address = realloc(elem_address, row_shift + action->field_size);

            if (NULL == elem_address) {
                result = -1; // errno set
                goto end;
            }

            memset(elem_address + row_shift, 0, action->field_size);

            *(void **) (address + action->rows_offset) = elem_address;
            elem_address += row_shift;

            result = yajp_parse_array_value_internal(data, name, action, elem_address);
            if (0 != result) {
                goto end;
            }

            row_shift += action->field_size;
            (*count)++;
        }

        if (YAJP_PARSER_RECOGNIZED_ENTITY_TYPE_VALUE == recognized_entity.type) {
            if (action->allocate_elems) {
                elem_address = *(void **) (address + action->elems_offset);
                elem_address = realloc(elem_address, row_shift + action->elem_size);

                if (NULL == elem_address) {
                    result = -1; // errno set
                    goto end;
                }

                *(void **) (address + action->elems_offset) = elem_address;
            } else {
                elem_address = address + action->elems_offset;
            }

            elem_address += row_shift;

            if (action->options & YAJP_DESERIALIZATION_TYPE_STRING) {
                void *str = malloc(recognized_entity.token->attributes.value_size + action->elem_size);
                if (NULL == str) {
                    result = -1; // errno set
                    goto end;
                }
                *(void **) (elem_address) = str;
                elem_address = str;
            }

            setter_result = action->setter(name->attributes.value, name->attributes.value_size,
                                           recognized_entity.token->attributes.value,
                                           recognized_entity.token->attributes.value_size, elem_address, data->user_data);

            if (0 != setter_result) {
                result = -1;// deserialization error
                goto end;
            }

            (*count)++;
            row_shift += action->elem_size;
        }

        if (i >= TOKEN_CNT) {
            yajp_lexer_release_token(&tokens[i % TOKEN_CNT]);
        }
    } while ((YAJP_PARSER_RECOGNIZED_ENTITY_TYPE_AEND != recognized_entity.type) &&
             (YAJP_PARSER_RECOGNIZED_ENTITY_TYPE_PAIR != recognized_entity.type) &&
             (YAJP_TOKEN_AEND != picked_token));

    end:
    for (i = 0; i < TOKEN_CNT; i++) {
        yajp_lexer_release_token(&tokens[i]);
    }
    return result;
#undef TOKENS_CNT
}

static int yajp_parse_object_value(yajp_deserialization_data_t *data, const yajp_deserialization_rule_t *action,
                                   void *address) {
    int result;

    if (action->allocate) {
        void *tmp = malloc(action->field_size);
        if (NULL == tmp) {
            return -1; // errno set
        }
        memset(tmp, 0, action->field_size);
        result = yajp_parse(data, action->ctx, tmp);

        if (0 != result) {
            free(tmp);
            return result;
        } else {
            *(void **) address = tmp;
        }
    } else {
        memset(address, 0, action->field_size);
        result = yajp_parse(data, action->ctx, address);
    }

    return result;
}

static int yajp_parse_array_of_objects_value(yajp_deserialization_data_t *data, const yajp_deserialization_rule_t *action,
                                             void *address) {
    yajp_lexer_token_t current_token;
    yajp_parser_recognized_entity_t recognized_entity;
    int result;

    yajp_lexer_get_next_token(data->lexer_input, &current_token);
    if (YAJP_TOKEN_ABEGIN != current_token.token) {
        result = -1; // expected [ token
        goto end;
    }

    yajp_parser_parse(data->parser, current_token.token, &current_token, &recognized_entity);

    if (action->allocate) {
        void *tmp = malloc(action->field_size);
        if (NULL == tmp) {
            return -1; // errno set
        }
        memset(tmp, 0, action->field_size);
        result = yajp_parse_array_of_objects_value_internal(data, action, tmp);

        if (0 != result) {
            free(tmp);
            goto end;
        } else {
            *(void **) address = tmp;
        }
    } else {
        memset(address, 0, action->field_size);
        result = yajp_parse_array_of_objects_value_internal(data, action, address);
    }

    end:
    yajp_lexer_release_token(&current_token);
    return result;
}

static int yajp_parse_array_of_objects_value_internal(yajp_deserialization_data_t *data,
                                                      const yajp_deserialization_rule_t *action, void *address) {
#define TOKEN_CNT 3
    int result = 0;
    yajp_lexer_token_t tokens[TOKEN_CNT];
    yajp_lexer_token_t *current_token;
    yajp_token_type_t picked_token;

    yajp_parser_recognized_entity_t recognized_entity;
    int i = 0;
    size_t row_shift = 0;
    void *elem_address;

    size_t *count = address + action->counter_offset;
    bool *final_dim = address + action->final_dym_offset;

    *final_dim = true;

    memset(tokens, 0, sizeof(tokens));

    do {
        current_token = &tokens[i % TOKEN_CNT];
        i++;

        yajp_lexer_get_next_token(data->lexer_input, current_token);
        picked_token = current_token->token;

        recognized_entity.type = YAJP_PARSER_RECOGNIZED_ENTITY_TYPE_NONE;
        yajp_parser_parse(data->parser, picked_token, current_token, &recognized_entity);

        if (YAJP_TOKEN_ABEGIN == picked_token) {
            *final_dim = false;

            elem_address = *(void **) (address + action->rows_offset);
            elem_address = realloc(elem_address, row_shift + action->field_size);

            if (NULL == elem_address) {
                result = -1; // errno set
                goto end;
            }

            memset(elem_address + row_shift, 0, action->field_size);

            *(void **) (address + action->rows_offset) = elem_address;
            elem_address += row_shift;

            result = yajp_parse_array_of_objects_value_internal(data, action, elem_address);
            if (0 != result) {
                goto end;
            }

            row_shift += action->field_size;
            (*count)++;
        }

        if (YAJP_PARSER_RECOGNIZED_ENTITY_TYPE_VALUE == recognized_entity.type || YAJP_TOKEN_OBEGIN == picked_token) {
            if (action->allocate_elems) {
                elem_address = *(void **) (address + action->elems_offset);
                elem_address = realloc(elem_address, row_shift + action->elem_size);

                if (NULL == elem_address) {
                    result = -1; //errno set
                    goto end;
                }

                *(void **) (address + action->elems_offset) = elem_address;
            } else {
                elem_address = address + action->elems_offset;
            }

            elem_address += row_shift;

            result = yajp_parse(data, action->ctx, elem_address);
            if (0 != result) {
                goto end;
            }

            (*count)++;
            row_shift += action->elem_size;
        }

        if (i >= TOKEN_CNT) {
            yajp_lexer_release_token(&tokens[i % TOKEN_CNT]);
        }
    } while ((YAJP_PARSER_RECOGNIZED_ENTITY_TYPE_AEND != recognized_entity.type) &&
             (YAJP_PARSER_RECOGNIZED_ENTITY_TYPE_PAIR != recognized_entity.type) &&
             (YAJP_TOKEN_AEND != picked_token));

end:
    for (i = 0; i < TOKEN_CNT; i++) {
        yajp_lexer_release_token(&tokens[i]);
    }
    return result;
#undef TOKENS_CNT
}
