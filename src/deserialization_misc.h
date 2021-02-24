/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * deserialization_misc.h
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


#ifndef YAJP_DESERIALIZATION_MISC_H
#define YAJP_DESERIALIZATION_MISC_H


#include <stddef.h>
#include <stdint.h>
#include "yajp/deserialization.h"

/**
 *
 * @param data
 * @param data_size
 * @return
 */
int yajp_calculate_hash(const char *data, size_t data_size);

/**
 *
 * @param ctx
 */
void yajp_sort_actions_in_context(yajp_deserialization_ctx_t *ctx);

/**
 *
 * @param ctx
 * @param name
 * @param name_size
 * @return
 */
const yajp_deserialization_action_t *yajp_find_action(const yajp_deserialization_ctx_t *ctx, const uint8_t *name,
                                                      size_t name_size);

#endif //YAJP_DESERIALIZATION_MISC_H
