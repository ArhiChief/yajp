/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * deserialization_routine.h
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


#ifndef YAJP_DESERIALIZATION_ROUTINE_H
#define YAJP_DESERIALIZATION_ROUTINE_H

#include <stdint.h>
#include <stdlib.h>

int yajp_parse_short(const uint8_t *name, size_t name_size, const uint8_t *value, size_t value_size, void *field, void *user_data);
int yajp_parse_int(const uint8_t *name, size_t name_size, const uint8_t *value, size_t value_size, void *field, void *user_data);
int yajp_parse_long_int(const uint8_t *name, size_t name_size, const uint8_t *value, size_t value_size, void *field, void *user_data);
int yajp_parse_long_long_int(const uint8_t *name, size_t name_size, const uint8_t *value, size_t value_size, void *field, void *user_data);

int yajp_parse_float(const uint8_t *name, size_t name_size, const uint8_t *value, size_t value_size, void *field, void *user_data);
int yajp_parse_double(const uint8_t *name, size_t name_size, const uint8_t *value, size_t value_size, void *field, void *user_data);
int yajp_parse_long_double(const uint8_t *name, size_t name_size, const uint8_t *value, size_t value_size, void *field, void *user_data);

int yajp_parse_bool(const uint8_t *name, size_t name_size, const uint8_t *value, size_t value_size, void *field, void *user_data);

int yajp_parse_string(const uint8_t *name, size_t name_size, const uint8_t *value, size_t value_size, void *field, void *user_data);

#endif //YAJP_DESERIALIZATION_ROUTINE_H
