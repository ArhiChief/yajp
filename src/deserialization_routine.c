/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * deserialization_routine.c
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
#include <errno.h>
#include <limits.h>
#include <stdbool.h>
#include <string.h>

#include "yajp/deserialization_routine.h"

int yajp_parse_short(const char *name, size_t name_size, const uint8_t *value, size_t value_size, void *field,
                     void *user_data) {
    uint8_t *pend;
    long result;

    (void)name;
    (void)name_size;
    (void)user_data;

    if (NULL != value && 0 < value_size) {
        result = strtol((char *)value, (char **)&pend, 10);

        if ((errno == ERANGE && (result == LONG_MAX || result == LONG_MIN)) || (errno != 0 && result == 0)) {
            return -1;
        }

        if (result < SHRT_MIN || result > SHRT_MAX) {
            return -1;
        }

        if (pend == value || pend != (value + (value_size / sizeof(*value)))) {
            return -1;
        }

        *((short *) field) = (short)result;
        return 0;
    }

    return -1;
}

int yajp_parse_int(const char *name, size_t name_size, const uint8_t *value, size_t value_size, void *field,
                   void *user_data) {

    uint8_t *pend;
    long long int result;

    (void)name;
    (void)name_size;
    (void)user_data;

    if (NULL != value && 0 < value_size) {
        result = strtoll((char *)value, (char **)&pend, 10);

        if ((errno == ERANGE && (result == INT_MAX || result == INT_MIN)) || (errno != 0 && result == 0)) {
            return -1;
        }

        if (result < INT_MIN || result > INT_MAX) {
            return -1;
        }

        if (pend == value || pend != (value + (value_size / sizeof(*value)))) {
            return -1;
        }

        *((int *) field) = (int)result;
        return 0;
    }

    return -1;
}

int yajp_parse_long_int(const char *name, size_t name_size, const uint8_t *value, size_t value_size, void *field,
                        void *user_data) {

    uint8_t *pend;
    long long result;

    (void)name;
    (void)name_size;
    (void)user_data;

    if (NULL != value && 0 < value_size) {
        result = strtol((char *)value, (char **)&pend, 10);

        if ((errno == ERANGE && (result == LONG_MAX || result == LONG_MIN)) || (errno != 0 && result == 0)) {
            return -1;
        }

        if (result < LONG_MIN || result > LONG_MAX) {
            return -1;
        }

        if (pend == value || pend != (value + (value_size / sizeof(*value)))) {
            return -1;
        }

        *((long *) field) = (long)result;
        return 0;
    }

    return -1;
}

int yajp_parse_long_long_int(const char *name, size_t name_size, const uint8_t *value, size_t value_size, void *field,
                             void *user_data) {

    uint8_t *pend;
    long long result;

    (void)name;
    (void)name_size;
    (void)user_data;

    if (NULL != value && 0 < value_size) {
        result = strtoll((char *)value, (char **)&pend, 10);

        if ((errno == ERANGE && (result == LLONG_MAX || result == LLONG_MIN)) || (errno != 0 && result == 0)) {
            return -1;
        }

        if (pend == value || pend != (value + (value_size / sizeof(*value)))) {
            return -1;
        }

        *((long long *) field) = result;
        return 0;
    }

    return -1;
}

int yajp_parse_float(const char *name, size_t name_size, const uint8_t *value, size_t value_size, void *field,
                     void *user_data) {

    uint8_t *pend;
    float result;

    (void)name;
    (void)name_size;
    (void)user_data;

    if (NULL != value && 0 < value_size) {
        result = strtof((char *)value, (char **)&pend);

        if (errno == ERANGE && (errno != 0 && result == 0)) {
            return -1;
        }

        if (pend == value || pend != (value + (value_size / sizeof(*value)))) {
            return -1;
        }

        *((float *) field) = result;
        return 0;
    }

    return -1;
}

int yajp_parse_double(const char *name, size_t name_size, const uint8_t *value, size_t value_size, void *field,
                      void *user_data) {

    uint8_t *pend;
    double result;

    (void)name;
    (void)name_size;
    (void)user_data;

    if (NULL != value && 0 < value_size) {
        result = strtod((char *)value, (char **)&pend);

        if (errno == ERANGE && (errno != 0 && result == 0)) {
            return -1;
        }

        if (pend == value || pend != (value + (value_size / sizeof(*value)))) {
            return -1;
        }

        *((double *) field) = result;
        return 0;
    }

    return -1;
}

int yajp_parse_long_double(const char *name, size_t name_size, const uint8_t *value, size_t value_size, void *field,
                           void *user_data) {

    uint8_t *pend;
    long double result;

    (void)name;
    (void)name_size;
    (void)user_data;

    if (NULL != value && 0 < value_size) {
        result = strtold((char *)value, (char **)&pend);

        if (errno == ERANGE && (errno != 0 && result == 0)) {
            return -1;
        }

        if (pend == value || pend != (value + (value_size / sizeof(*value)))) {
            return -1;
        }

        *((long double *) field) = result;
        return 0;
    }

    return -1;
}


int yajp_parse_bool(const char *name, size_t name_size, const uint8_t *value, size_t value_size, void *field,
                    void *user_data) {

    bool result;
    static const char true_str[] = {'t','r', 'u', 'e'};
    static const size_t true_str_size = sizeof(true_str);
    static const char false_str[] = {'f','a', 'l', 's', 'e'};
    static const size_t false_str_size = sizeof(false_str);

    (void)name;
    (void)name_size;
    (void)user_data;

    if (NULL != value) {
        if (value_size == true_str_size && (0 == memcmp(value, true_str, true_str_size))) {
            *((bool *)field) = true;
            return 0;
        } else if (value_size == false_str_size && (0 == memcmp(value, false_str, false_str_size))) {
            *((bool *)field) = false;
            return 0;
        }
    }

    return -1;
}