/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * deserialization_misc.c
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


#include <stdlib.h>
#include <string.h>

#include "deserialization_misc.h"

unsigned long yajp_calculate_hash(const uint8_t *data, size_t data_size) {
    unsigned long hash = 5381;

    if (NULL == data || 0 == data_size) {
        return 0;
    }

    while (data_size--) {
        hash = ((hash << 5) + hash) + *data++;
    }

    return hash;
}

static int actions_comparator(const void *pkey, const void *pelem) {
    return ((const yajp_deserialization_action_t *)pkey)->field_key - ((const yajp_deserialization_action_t *)pelem)->field_key;
}

void yajp_sort_actions_in_context(yajp_deserialization_ctx_t *ctx) {
    if (NULL != ctx->actions && 1 < ctx->actions_cnt) {
        qsort(ctx->actions, ctx->actions_cnt, sizeof(*ctx->actions), actions_comparator);
    }
}

const yajp_deserialization_action_t *yajp_find_action_in_context(const yajp_deserialization_ctx_t *ctx,
                                                                 const uint8_t *name, size_t name_size) {
    const yajp_deserialization_action_t *action = NULL;
    const yajp_deserialization_action_t key = { .field_key = yajp_calculate_hash(name, name_size) };

    if (NULL != ctx && NULL != ctx->actions && 0 < ctx->actions_cnt) {
        action = bsearch(&key, ctx->actions, ctx->actions_cnt, sizeof(*ctx->actions), actions_comparator);
    }

    return action;
}

int yajp_deserialization_ctx_init(yajp_deserialization_action_t *acts, int count, yajp_deserialization_ctx_t *ctx) {
    memset(ctx, 0, sizeof(*ctx));

    ctx->actions = acts;
    ctx->actions_cnt = count;

    yajp_sort_actions_in_context(ctx);

    return 0;
}

char *yajp_deserialization_result_status_to_str(const yajp_deserialization_result_t *result) {
#define STR_UNEXPECTED_EOF              "Unexpected end of stream"
#define STR_DESERIALIZATION_SUCCEED     "Success"
#define STR_UNRECOGNIZED_TOKEN          "Unrecognized token found"

#ifdef YAJP_TRACK_STREAM
    #define STR_DESERIALIZATION_FAILED  "Failed to deserialize field at line: %d, column: %d"
    #define STR_UNEXPECTED_TOKEN        "Unexpected token found, expected: '%s', found: '%s' at line: %d, column: %d";
    static const size_t description_len = sizeof(STR_UNEXPECTED_TOKEN) + (7 + 7 + 5 + 5) * sizeof(char);
#else
    #define STR_DESERIALIZATION_FAILED  "Failed to deserialize field"
    #define STR_UNEXPECTED_TOKEN        "Unexpected token found, expected: '%s', found: '%s'"
    static const size_t description_len = sizeof(STR_UNEXPECTED_TOKEN) + (7 + 7) * sizeof(char);
#endif

    char *description = NULL;
    const char *expected;
    const char *found;

    if (NULL == (description = malloc(description_len))) {
        return NULL;
    }

    if (YAJP_DESERIALIZATION_RESULT_STATUS_OK == result->status) {
        memmove(description, STR_DESERIALIZATION_SUCCEED, sizeof(STR_DESERIALIZATION_SUCCEED));
        return description;
    }

    if (0 != (result->status & YAJP_DESERIALIZATION_RESULT_STATUS_UNEXPECTED_EOF)) {
        memmove(description, STR_UNEXPECTED_EOF, sizeof(STR_UNEXPECTED_EOF));
        return description;
    }

    if (0 != (result->status & YAJP_DESERIALIZATION_RESULT_STATUS_UNRECOGNIZED_TOKEN)) {
        memmove(description, STR_UNRECOGNIZED_TOKEN, sizeof(STR_UNRECOGNIZED_TOKEN));
        return description;
    }

    if (0 != (result->status & YAJP_DESERIALIZATION_RESULT_STATUS_DESERIALIZATION_ERROR)) {
#ifdef YAJP_TRACK_STREAM
        snprintf(description, description_len, STR_DESERIALIZATION_FAILED, result->line_num, result->column_num);
#else
        snprintf(description, description_len, STR_DESERIALIZATION_FAILED);
#endif
        return description;
    }

    switch (result->status & 0x0F0) {
        case YAJP_DESERIALIZATION_RESULT_STATUS_EXPECTED_OBEGIN:
            expected = "{";
            break;
        case YAJP_DESERIALIZATION_RESULT_STATUS_EXPECTED_OEND:
            expected = "}";
            break;
        case YAJP_DESERIALIZATION_RESULT_STATUS_EXPECTED_ABEGIN:
            expected = "[";
            break;
        case YAJP_DESERIALIZATION_RESULT_STATUS_EXPECTED_AEND:
            expected = "]";
            break;
        case YAJP_DESERIALIZATION_RESULT_STATUS_EXPECTED_FIELD:
            expected = "field";
            break;
        case YAJP_DESERIALIZATION_RESULT_STATUS_EXPECTED_VALUE:
            expected = "value";
            break;
        case YAJP_DESERIALIZATION_RESULT_STATUS_EXPECTED_COMMA:
            expected = ",";
            break;
        case YAJP_DESERIALIZATION_RESULT_STATUS_EXPECTED_COLON:
            expected = ":";
            break;
        default:
            expected = "Unknown";
            break;
    }

    switch (result->status & 0x00F) {
        case YAJP_DESERIALIZATION_RESULT_STATUS_FOUND_OBEGIN:
            found = "{";
            break;
        case YAJP_DESERIALIZATION_RESULT_STATUS_FOUND_OEND:
            found = "}";
            break;
        case YAJP_DESERIALIZATION_RESULT_STATUS_FOUND_ABEGIN:
            found = "[";
            break;
        case YAJP_DESERIALIZATION_RESULT_STATUS_FOUND_AEND:
            found = "]";
            break;
        case YAJP_DESERIALIZATION_RESULT_STATUS_FOUND_FIELD:
            found = "field";
            break;
        case YAJP_DESERIALIZATION_RESULT_STATUS_FOUND_VALUE:
            found = "value";
            break;
        case YAJP_DESERIALIZATION_RESULT_STATUS_FOUND_COMMA:
            found = ",";
            break;
        case YAJP_DESERIALIZATION_RESULT_STATUS_FOUND_COLON:
            found = ":";
            break;
        default:
            found = "Unknown";
            break;
    }

#ifdef YAJP_TRACK_STREAM
    snprintf(description, description_len, STR_UNEXPECTED_TOKEN, expected, found, result->line_num, result->column_num);
#else
    snprintf(description, description_len, STR_UNEXPECTED_TOKEN, expected, found);
#endif

    return description;

#undef STR_UNEXPECTED_EOF
#undef STR_DESERIALIZATION_SUCCEED
#undef STR_DESERIALIZATION_FAILED
#undef STR_UNEXPECTED_TOKEN
}

int yajp_deserialization_action_init(const char *field_name,
                                     size_t name_size,
                                     size_t offset,
                                     size_t field_size,
                                     yajp_deserialization_action_options_t options,
                                     yajp_value_setter_t setter,
                                     yajp_deserialization_action_t *result) {

    memset(result, 0, sizeof(*result));

    result->field_key = yajp_calculate_hash(field_name, name_size);
    result->size = field_size;
    result->offset = offset;
    result->options = options;

    switch (options) {
        case YAJP_DESERIALIZATION_ACTION_OPTIONS_TYPE_PRIMITIVE:
            result->option_params.field_primitive.setter = setter;
            break;
        case YAJP_DESERIALIZATION_ACTION_OPTIONS_TYPE_STRING | YAJP_DESERIALIZATION_ACTION_OPTIONS_ALLOCATE:
            result->option_params.field_string.allocate = true;
        case YAJP_DESERIALIZATION_ACTION_OPTIONS_TYPE_STRING:
            result->option_params.field_string.setter = setter;
            break;
        default:
            return -1;
    }

    return 0;
}

int yajp_deserialization_array_action_init(const char *field_name,
                                           size_t name_size,
                                           size_t field_offset,
                                           size_t field_size,
                                           yajp_deserialization_action_options_t options,
                                           size_t counter_offset,
                                           size_t final_dim_offset,
                                           size_t rows_offset,
                                           size_t elem_size,
                                           size_t elems_offset,
                                           yajp_value_setter_t setter,
                                           yajp_deserialization_action_t *result) {

    memset(result, 0, sizeof(*result));

    result->field_key = yajp_calculate_hash(field_name, name_size);
    result->size = field_size;
    result->offset = field_offset;
    result->options = options;

    return 0;
}