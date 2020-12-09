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

const yajp_deserialization_action_t *yajp_get_action(const yajp_deserialization_ctx_t *ctx,
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

int yajp_deserialization_action_init(const char *field_name, size_t name_size, size_t offset, size_t field_size,
                                     yajp_deserialization_action_type_t action_type, yajp_value_setter_t setter,
                                     yajp_deserialization_action_t *result) {
    memset(result, 0, sizeof(*result));

    result->field_key = yajp_calculate_hash(field_name, name_size);
    result->size = field_size;
    result->offset = offset;
    result->type = action_type;

    switch (action_type) {
        case YAJP_DESERIALIZATION_ACTION_TYPE_FIELD:
            result->setter = setter;
            break;
        default:
            return -1;
    }

    return 0;
}