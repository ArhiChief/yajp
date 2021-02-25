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

int yajp_calculate_hash(const char *data, size_t data_size) {
    int hash = 5381;

    if (NULL == data || 0 == data_size) {
        return 0;
    }

    while (data_size--) {
        hash = ((hash << 5) + hash) + *data++;
    }

    return hash;
}

static int actions_comparator(const void *pkey, const void *pelem) {
    return ((const yajp_deserialization_action_t *) pkey)->field_key -
           ((const yajp_deserialization_action_t *) pelem)->field_key;
}

void yajp_sort_actions_in_context(yajp_deserialization_ctx_t *ctx) {
    if (NULL != ctx->actions && 1 < ctx->actions_cnt) {
        qsort(ctx->actions, ctx->actions_cnt, sizeof(*ctx->actions), actions_comparator);
    }
}

const yajp_deserialization_action_t *yajp_find_action(const yajp_deserialization_ctx_t *ctx, const uint8_t *name,
                                                      size_t name_size) {
    const yajp_deserialization_action_t *action = NULL;
    const yajp_deserialization_action_t key = {.field_key = yajp_calculate_hash(name, name_size)};

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

int yajp_deserialization_action_init(const char *name,
                                       size_t name_size,
                                       size_t field_offset,
                                       size_t field_size,
                                       int options,
                                       size_t counter_offset,
                                       size_t final_dim_offset,
                                       size_t rows_offset,
                                       size_t elems_offset,
                                       size_t elem_size,
                                       yajp_value_setter_t setter,
                                       const yajp_deserialization_ctx_t *ctx,
                                       yajp_deserialization_action_t *result) {

    result->options = options;
    result->field_key = yajp_calculate_hash(name, name_size);
    result->field_size = field_size;
    result->field_offset = field_offset;

    result->allocate = options & (YAJP_DESERIALIZATION_OPTIONS_ALLOCATE | YAJP_DESERIALIZATION_TYPE_NULLABLE);
    result->allocate_elems = options & YAJP_DESERIALIZATION_OPTIONS_ALLOCATE_ELEMENTS;

    if (options & YAJP_DESERIALIZATION_TYPE_ARRAY_OF) {
        result->counter_offset = counter_offset;
        result->final_dym_offset = final_dim_offset;
        result->rows_offset = rows_offset;
        result->elems_offset = elems_offset;

    }

    result->elem_size = (options & (YAJP_DESERIALIZATION_TYPE_ARRAY_OF | YAJP_DESERIALIZATION_TYPE_STRING))
            ? elem_size
            : 0;

    result->ctx = ctx;
    result->setter = setter;

    return 0;
}