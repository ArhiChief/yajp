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
#include <stdint.h>
#include <stdbool.h>

/**
 *  Prototype of function used to convert string value into structure field type
 *
 *  @param[in]      name        Pointer to string with name of field
 *  @param[in]      name_size   Size of field name string in bytes
 *  @param[in]      value       Pointer to string with field value
 *  @param[in]      value_size  Size of value string in bytes
 *  @param[out]     field       Pointer to field in structure
 *  @param[in, out] user_data   Pointer to user data passed as parameter to deserialization functions
 */
typedef int (*yajp_value_setter_t)(const uint8_t *name,
                                   size_t name_size,
                                   const uint8_t *value,
                                   size_t value_size,
                                   void *field,
                                   void *user_data
);



struct yajp_deserialization_ctx;
struct yajp_deserialization_action;

typedef struct yajp_deserialization_ctx yajp_deserialization_ctx_t;
typedef struct yajp_deserialization_action yajp_deserialization_action_t;

/**
 * Deserialization context
 */
struct yajp_deserialization_ctx {
   int actions_cnt;
   yajp_deserialization_action_t *actions;
};


/**
 * @details     @c YAJP_DESERIALIZATION_FIELD_TYPE declaration value used to specify that deserializing type is number (integral or real)
 */
#define YAJP_DESERIALIZATION_TYPE_NUMBER                0b00000001
/**
 * @details     @c YAJP_DESERIALIZATION_FIELD_TYPE declaration value used to specify that deserializing type is string.
 */
#define YAJP_DESERIALIZATION_TYPE_STRING                0b00000010
/**
 * @details     @c YAJP_DESERIALIZATION_FIELD_TYPE declaration value used to specify that deserializing type is boolean
 */
#define YAJP_DESERIALIZATION_TYPE_BOOLEAN               0b00000100
/**
 * @details     @c YAJP_DESERIALIZATION_FIELD_TYPE declaration value used to specify that deserializing type is object
 */
#define YAJP_DESERIALIZATION_TYPE_OBJECT                0b00001000

/**
 * @details     Type modification. Specify that value can be null.
 *
 * @note    Can be combined with @c YAJP_DESERIALIZATION_TYPE_NULLABLE and one of YAJP_DESERIALIZATION_TYPE_XXX
 */
#define YAJP_DESERIALIZATION_TYPE_ARRAY_OF              0b00010000
/**
 * @details     Type modification. Specify that type is array of @c YAJP_DESERIALIZATION_TYPE_XXX
 *
 * @note    Field in structure should be represented as pointer. Otherwise compilation error will occur
 * @note    Can be combined with @c YAJP_DESERIALIZATION_TYPE_ARRAY_OF and one of YAJP_DESERIALIZATION_TYPE_XXX
 */
#define YAJP_DESERIALIZATION_TYPE_NULLABLE              0b00100000

/**
 * @details. Deserialization option. Tels YAJP what value for field should be allocated on heap
 *
 * @note    Field in structure should be represented as pointer. Otherwise compilation error will occur
 */
#define YAJP_DESERIALIZATION_OPTIONS_ALLOCATE           0b01000000
/**
 * @details In case if @c YAJP_DESERIALIZATION_FIELD_TYPE contains @c YAJP_DESERIALIZATION_TYPE_ARRAY_OF specify tells
 *          YAJP what memory for elements should be allocated on heap. Ignored otherwise
 */
#define YAJP_DESERIALIZATION_OPTIONS_ALLOCATE_ELEMENTS  0b10000000


/**
 * Description of deserialization action
 */
struct yajp_deserialization_action {
    int field_key;                              // hash of field used for fast search

    size_t field_offset;                        // offset of field in structure
    size_t field_size;                          // size of field

    int options;                                // deserialization options

    bool allocate;                              // memory allocation required for this deserializing field
    bool allocate_elems;                        // allocation for array values needed

    size_t counter_offset;                      // offset of counter
    size_t rows_offset;                         // offset of rows array
    size_t elems_offset;                        // offset of array values
    size_t final_dym_offset;                    // offset of final_dim flag
    size_t elem_size;                           // size of array element

    union {
        yajp_value_setter_t setter;             // pointer to setter function
        const yajp_deserialization_ctx_t *ctx;  // deserialization context
    };
};


/**
 * @details     Initialize instance of deserialization action for field in deserializing structure.
 *
 * @param [in]  name                Name of field in JSON stream
 * @param [in]  name_size           Size of field in JSON stream without '\0' in bytes
 * @param [in]  field_offset        Offset of deserializing field in deserializing structure in bytes
 * @param [in]  field_size          Size of deserializing field in deserializing structure in bytes
 * @param [in]  options             Deserialization action options
 * @param [in]  counter_offset      Offset of @e counter field inside array holder structure in bytes
 * @param [in]  final_dim_offset    Offset of @e final @e dimension @e flag field inside array holder structure in bytes
 * @param [in]  rows_offset         Offset of @e rows field inside array holder structure in bytes
 * @param [in]  elems_offset        Offset of @e elements field inside array holder structure in bytes
 * @param [in]  elem_size           Size of array element or string character
 * @param [in]  setter              Pointer to function used to deserialize JSON and init appropriate structure field
 * @param [in]  ctx                 Pointer to deserialization context used to deserialize inherit object
 * @param [out] result              Pointer to resulting deserialization action
 *
 * @return  Deserialization action initialization result. 0 - on success
 *
 * @note    This function is very generic. Consider using declarative API.
 * @note    Parameters @b counter_offset, @b final_dim_offset, @b rows_offset, @b elems_offset are ignored if @b options
 *          doesn't have @c YAJP_DESERIALIZATION_ACTION_OPTIONS_TYPE_ARRAY_OF
 * @note    Value in @b elem_size should be size in bytes of string character in case of string deserialization or size
 *          of array item in case of array deserialization
 */
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
                                     yajp_deserialization_action_t *result);

/**
 * Initialize deserialization context
 * @param[in]   acts    Pointer to array of deserialization action
 * @param[in]   count   Number of deserialization action in array
 * @param[out]  ctx     Pointer to initializing deserialization context
 * @return      Result of deserialization context initialization. 0 on success
 */
int yajp_deserialization_ctx_init(yajp_deserialization_action_t *acts, int count, yajp_deserialization_ctx_t *ctx);

/**
 * Deserialize JSON stream into provided structure
 * @param[in]   json                    Pointer to JSON stream
 * @param[in]   ctx                     Pointer to deserialization context
 * @param[out]  deserializing_struct    Pointer to deserializing structure
 * @param[in]   user_data               Pointer to value what will be passed as \c user_data to \c setter in \c yajp_deserialization_action_init
 * @return      Result of deserialization process. See \c yajp_deserialization_result_t for details
 */
int yajp_deserialize_json_stream(FILE *json,
                                 const yajp_deserialization_ctx_t *ctx,
                                 void *deserializing_struct,
                                 void *user_data);

/**
 * Deserialize plain JSON string into provided structure
 * @param[in]   json                    Pointer to string with JSON
 * @param[in]   json_size               Size in bytes of deserializing JSON string
 * @param[in]   ctx                     Pointer to deserialization context
 * @param[out]  deserializing_struct    Pointer to deserializing structure
 * @param[in]   user_data               Pointer to value what will be passed as \c user_data to \c setter in \c yajp_deserialization_action_init
 * @return      Result of deserialization process. See \c yajp_deserialization_result_t for details
 *
 * @note    This function wraps passed JSON string into \c FILE using \c fmemopen call from \c stdio.h
 */
int yajp_deserialize_json_string(const char *json,
                                 size_t json_size,
                                 const yajp_deserialization_ctx_t *ctx,
                                 void *deserializing_struct,
                                 void *user_data);



/**
 *
 * Generic function to initialize deserialization action for arrays
 *
 * @details
 * Array deserialization is a complex process because nigher size of array, nor amount of array dimensions is unknown
 * until deserialization ends. Even if user knows array parameters it's impossible to make it more easier because JSON
 * supports arrays with non-constant amount of elements in each dimensions. To cover this case, YAJP expects that
 * deserializing array will be represented as a struct used to hold dynamic arrays, similar to something like this:
 * @code
 *  typedef struct test_struct_arr test_struct_arr_t;
 *  struct test_struct_arr {
 *      union {  // union here can be replaced with struct if you want. Union is used to reduce amount of space
 *          int *elems; // pointer to array used to store deserializing array values
 *          test_struct_arr_t *rows; // pointer to rows if array is multidimensional
 *       };
 *       bool final_dim; // flag used to determine what structure holds array values. `false` if struct holds rows
 *       size_t count; // amount of items in `elems` or `rows`
 *   };
 * @endcode
 * `final_dim` require additional attention. In case if YAJP deserializing matrix (2-dim array) next peace of code should
 * works fine:
 * @code
 * typedef struct {
 *  test_struct_arr_t arr;
 * } deserializing_struct_t;
 *
 * // field declaration
 * deserializing_struct_t test_struct;
 *
 * // general deserialization routines
 *
 * rows_count = test_struct.arr.count;   // will show amount of rows in matrix
 *
 * for (i = 0; i < rows_count; i++) {   // traverse matrix
 *  assert(test_struct.arr.final_dim == false);
 *  columns_count = test_struct.arr.rows[i].count; // amount of elements in row
 *  assert(test_struct.arr.rows[i].final_dim == true);
 *  for (int j = 0; j < columns_count; j++) {
 *      // do whatever we want with matrix element
 *      value = test_struct.arr.rows[i].elems[j];
 *  }
 * }
 * @endcode
 *
 * @param[in]   field_name          Name of field in JSON stream
 * @param[in]   name_size           Size of field name in JSON without '\0' character
 * @param[in]   field_offset        Offset of field in deserializing structure
 * @param[in]   field_size          Size of field in deserializing structure
 * @param[in]   options             Deserialization options for field in deserialization process
 * @param[in]   counter_offset      Offset of counter field
 * @param[in]   final_dim_offset    Offset to flag used to point that structure points to elements
 * @param[in]   rows_offset         Offset of field used to store other dimensions
 * @param[in]   elem_size           Size of array item
 * @param[in]   elems_offset        Offset of field to store array items
 * @param[in]   setter              Pointer to function used to deserialize JSON value into structure field
 * @param[out]  action              Pointer to initializing deserialization action
 *
 * @return  Result of initialization. 0 on success
 *
 * @note    Using this function is not convenient way. Prefer to use macro wrappers.
 */

#endif // YAJP_DESERIALIZE_H