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

/**
 * Function will convert passed string value to short and initialize passed deserializing field with this value.
 *
 * @param[in] name          Pointer to string with name of field where value should be set. Not used.
 * @param[in] name_size     Size of name field in bytes. Not used.
 * @param[in] value         Pointer to string with value for field.
 * @param[in] value_size    Size of string with value
 * @param[in] field         Pointer to field what should be set.
 * @param[in] user_data     Pointer to user data passed as parameter to deserialization functions. Not used
 * @return      Result of converting and setting string value to field. 0 - on success
 *
 * @note It's a wrapper over standard \c strtol() call. With range checks
 */
int yajp_set_short(const uint8_t *name, size_t name_size, const uint8_t *value, size_t value_size, void *field, void *user_data);
/**
 * Function will convert passed string value to int and initialize passed deserializing field with this value.
 *
 * @param[in] name          Pointer to string with name of field where value should be set. Not used.
 * @param[in] name_size     Size of name field in bytes. Not used.
 * @param[in] value         Pointer to string with value for field.
 * @param[in] value_size    Size of string with value
 * @param[in] field         Pointer to field what should be set.
 * @param[in] user_data     Pointer to user data passed as parameter to deserialization functions. Not used
 * @return      Result of converting and setting string value to field. 0 - on success
 *
 * @note It's a wrapper over standard \c strtol() call. With range checks
 */
int yajp_set_int(const uint8_t *name, size_t name_size, const uint8_t *value, size_t value_size, void *field, void *user_data);
/**
 * Function will convert passed string value to long int and initialize passed deserializing field with this value.
 *
 * @param[in] name          Pointer to string with name of field where value should be set. Not used.
 * @param[in] name_size     Size of name field in bytes. Not used.
 * @param[in] value         Pointer to string with value for field.
 * @param[in] value_size    Size of string with value
 * @param[in] field         Pointer to field what should be set.
 * @param[in] user_data     Pointer to user data passed as parameter to deserialization functions. Not used
 * @return      Result of converting and setting string value to field. 0 - on success
 *
 * @note It's a wrapper over standard \c strtol() call. With range checks
 */
int yajp_set_long_int(const uint8_t *name, size_t name_size, const uint8_t *value, size_t value_size, void *field, void *user_data);
/**
 * Function will convert passed string value to long long int and initialize passed deserializing field with this value.
 *
 * @param[in] name          Pointer to string with name of field where value should be set. Not used.
 * @param[in] name_size     Size of name field in bytes. Not used.
 * @param[in] value         Pointer to string with value for field.
 * @param[in] value_size    Size of string with value
 * @param[in] field         Pointer to field what should be set.
 * @param[in] user_data     Pointer to user data passed as parameter to deserialization functions. Not used
 * @return      Result of converting and setting string value to field. 0 - on success
 *
 * @note It's a wrapper over standard \c strtol() call. With range checks
 */
int yajp_set_long_long_int(const uint8_t *name, size_t name_size, const uint8_t *value, size_t value_size, void *field, void *user_data);

/**
 * Function will convert passed string value to float and initialize passed deserializing field with this value.
 *
 * @param[in] name          Pointer to string with name of field where value should be set. Not used.
 * @param[in] name_size     Size of name field in bytes. Not used.
 * @param[in] value         Pointer to string with value for field.
 * @param[in] value_size    Size of string with value
 * @param[in] field         Pointer to field what should be set.
 * @param[in] user_data     Pointer to user data passed as parameter to deserialization functions. Not used
 * @return      Result of converting and setting string value to field. 0 - on success
 *
 * @note It's a wrapper over standard \c strtod() call. With range checks
 */
int yajp_set_float(const uint8_t *name, size_t name_size, const uint8_t *value, size_t value_size, void *field, void *user_data);
/**
 * Function will convert passed string value to double and initialize passed deserializing field with this value.
 *
 * @param[in] name          Pointer to string with name of field where value should be set. Not used.
 * @param[in] name_size     Size of name field in bytes. Not used.
 * @param[in] value         Pointer to string with value for field.
 * @param[in] value_size    Size of string with value
 * @param[in] field         Pointer to field what should be set.
 * @param[in] user_data     Pointer to user data passed as parameter to deserialization functions. Not used
 * @return      Result of converting and setting string value to field. 0 - on success
 *
 * @note It's a wrapper over standard \c strtod() call. With range checks
 */
int yajp_set_double(const uint8_t *name, size_t name_size, const uint8_t *value, size_t value_size, void *field, void *user_data);
/**
 * Function will convert passed string value to long double and initialize passed deserializing field with this value.
 *
 * @param[in] name          Pointer to string with name of field where value should be set. Not used.
 * @param[in] name_size     Size of name field in bytes. Not used.
 * @param[in] value         Pointer to string with value for field.
 * @param[in] value_size    Size of string with value
 * @param[in] field         Pointer to field what should be set.
 * @param[in] user_data     Pointer to user data passed as parameter to deserialization functions. Not used
 * @return      Result of converting and setting string value to field. 0 - on success
 *
 * @note It's a wrapper over standard \c strtod() call. With range checks
 */
int yajp_set_long_double(const uint8_t *name, size_t name_size, const uint8_t *value, size_t value_size, void *field, void *user_data);

/**
 * Function will convert passed string value to boolean and initialize passed deserializing field with this value.
 *
 * @param[in] name          Pointer to string with name of field where value should be set. Not used.
 * @param[in] name_size     Size of name field in bytes. Not used.
 * @param[in] value         Pointer to string with value for field.
 * @param[in] value_size    Size of string with value
 * @param[in] field         Pointer to field what should be set.
 * @param[in] user_data     Pointer to user data passed as parameter to deserialization functions. Not used
 * @return      Result of converting and setting string value to field. 0 - on success
 *
 * @note This function compares content of value with `true` or `false`
 */
int yajp_set_bool(const uint8_t *name, size_t name_size, const uint8_t *value, size_t value_size, void *field, void *user_data);

/**
 * Function will convert passed string value to string and initialize passed deserializing field with this value.
 *
 * @param[in] name          Pointer to string with name of field where value should be set. Not used.
 * @param[in] name_size     Size of name field in bytes. Not used.
 * @param[in] value         Pointer to string with value for field.
 * @param[in] value_size    Size of string with value
 * @param[in] field         Pointer to field what should be set.
 * @param[in] user_data     Pointer to user data passed as parameter to deserialization functions. Not used
 * @return      Result of converting and setting string value to field. 0 - on success
 *
 * @note This function expects what size of memory region pointed by @c field will be at least @c (1 * sizeof(char))
 *       larger than @c value_size because it will add '\0' symbol to the end of string.
 */
int yajp_set_string(const uint8_t *name, size_t name_size, const uint8_t *value, size_t value_size, void *field, void *user_data);

#endif //YAJP_DESERIALIZATION_ROUTINE_H
