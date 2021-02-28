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

#include "khash.h"
#include "deserialization_misc.h"

field_key_t yajp_calculate_hash(const uint8_t *data, size_t data_size) {
    return __ac_X31_hash_string(data, data_size);
}

KHASH_MAP_INIT_INT(yajp, const yajp_deserialization_rule_t *)

const yajp_deserialization_rule_t *yajp_find_action(const yajp_deserialization_context_t *ctx, const uint8_t *name,
                                                    size_t name_size) {
    const yajp_deserialization_rule_t *action = NULL;
    field_key_t key = yajp_calculate_hash(name, name_size);
    khash_t(yajp) *hashmap = (khash_t(yajp) *)ctx->rules;
    khiter_t iterator;

    iterator = kh_get(yajp, hashmap, key);
    if (kh_exist(hashmap, iterator)) {
        action = kh_value(hashmap, iterator);
    }

    return action;
}

int yajp_deserialization_context_init(yajp_deserialization_rule_t *acts, int count, yajp_deserialization_context_t *ctx) {
    int ret, i;
    khash_t(yajp) *hashmap = kh_init(yajp);
    field_key_t key;
    khiter_t iterator;

    kh_resize(yajp, hashmap, count);

    for (i = 0; i < count; i++) {
        key = acts[i].field_key;
        iterator = kh_put(yajp, hashmap, key, &ret);
        if (!ret) {
            kh_del(yajp, hashmap, iterator);
            kh_destroy(yajp, hashmap);
            goto end;
        }

        kh_value(hashmap, iterator) = &acts[i];
    }

    ctx->rules = hashmap;

end:
    return (!ret) ? -1 : 0;
}

int yajp_deserialization_rule_init(const char *name,
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
                                   const yajp_deserialization_context_t *ctx,
                                   yajp_deserialization_rule_t *result) {

    result->options = options;
#if DEBUG
    result->field_name = name;
    result->field_name_size = name_size;
#endif
    result->field_key = yajp_calculate_hash((const uint8_t *)name, name_size);
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

    if ((NULL == setter) ^ (NULL == ctx)) {
        if (setter) {
            result->setter = setter;
        } else {
            result->ctx = ctx;
        }
    } else {
        return -1;
    }

    return 0;
}