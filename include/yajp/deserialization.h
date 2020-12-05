/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * parser.h
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


#ifndef YAJP_DESERIALIZE_H
#define YAJP_DESERIALIZE_H

#include <stdio.h>

typedef int (*yajp_value_setter_t)(const char *name,
                                   size_t name_size,
                                   const char *value,
                                   size_t value_size,
                                   void *field,
                                   void *user_data
);

typedef struct yajp_deserialization_ctx {
   int a;       /**< pepper */
} yajp_deserialization_ctx_t;

/**
 *
 * @param json
 * @param ctx
 * @param result
 * @param user_data
 * @return
 */
int yajp_deserialize_json_stream(FILE *json,
                                 const yajp_deserialization_ctx_t *ctx,
                                 void *result,
                                 void *user_data);

/**
 *
 * @param json
 * @param json_size
 * @param ctx
 * @param result
 * @param user_data
 * @return
 */
int yajp_deserialize_json_string(const char *json,
                                 size_t json_size,
                                 const yajp_deserialization_ctx_t *ctx,
                                 void *result,
                                 void *user_data);

#endif //YAJP_DESERIALIZE_H
