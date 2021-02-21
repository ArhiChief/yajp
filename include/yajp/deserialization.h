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
 * Deserializing option flags
 */
typedef enum yajp_deserialization_action_options {
    YAJP_DESERIALIZATION_ACTION_OPTIONS_NONE                = 0x000,
    // type of field
    YAJP_DESERIALIZATION_ACTION_OPTIONS_TYPE_PRIMITIVE      = 0x010,    // deserializing element is primitive
    YAJP_DESERIALIZATION_ACTION_OPTIONS_TYPE_STRING         = 0x020,    // deserializing element is string
    YAJP_DESERIALIZATION_ACTION_OPTIONS_TYPE_OBJECT         = 0x040,    // deserializing element is object

    YAJP_DESERIALIZATION_ACTION_OPTIONS_TYPE_ARRAY_OF       = 0x001,    // deserializing element is array. used with combination of other options

    YAJP_DESERIALIZATION_ACTION_OPTIONS_ALLOCATE            = 0x100,    // memory for field should be allocated
    YAJP_DESERIALIZATION_ACTION_OPTIONS_ALLOCATE_ELEMENTS   = 0x200,    // memory for array items should be allocated
} yajp_deserialization_action_options_t;

/**
 * Description of deserialization action
 */
struct yajp_deserialization_action {
    size_t offset;                                  // offset of field in structure
    size_t size;                                    // size of field
    yajp_deserialization_action_options_t options;  // deserialization options
    unsigned long field_key;                        // hash of field used for fast search

    union {
        struct {
            yajp_value_setter_t setter;             // pointer to setter function
            bool allocate;                          // memory allocation required for this deserializing field
        } primitive_field;                          // for primitive fields (boolean, number)
        struct {
            yajp_value_setter_t setter;             // pointer to setter function
            bool allocate;                          // allocation of array holder needed
            bool allocate_elems;                    // allocation for array values needed
            size_t counter_offset;                  // offset of counter
            size_t rows_offset;                     // offset of rows array
            size_t elems_offset;                    // offset of array values
            size_t final_dym_offset;                // offset of final_dim flag
            size_t elem_size;                       // size of array element
        } array_field;                              // for array fields
        struct {
            const yajp_deserialization_ctx_t *ctx;  // deserialization context
            bool allocate;                          // memory allocation required for this deserializing field
        } object_filed;
        struct {
            const yajp_deserialization_ctx_t *ctx;  // deserialization context
            bool allocate;                          // allocation of array holder needed
            bool allocate_elems;                    // allocation for array values needed
            size_t counter_offset;                  // offset of counter
            size_t rows_offset;                     // offset of rows array
            size_t elems_offset;                    // offset of array values
            size_t final_dym_offset;                // offset of final_dim flag
            size_t elem_size;                       // size of array element
        } array_of_objects_field;
    } option_params;                                // deserialization options data
};

/**
 * Represent status of JSON stream deserialization
 */
typedef enum yajp_deserialization_result_status {
    YAJP_DESERIALIZATION_RESULT_STATUS_OK                       = 0x000, // Deserialization succeeded
    YAJP_DESERIALIZATION_RESULT_STATUS_DESERIALIZATION_ERROR    = 0x100, // Failed to deserialize value from JSON stream
    YAJP_DESERIALIZATION_RESULT_STATUS_UNEXPECTED_EOF           = 0x200, // Unexpected end of JSON stream
    YAJP_DESERIALIZATION_RESULT_STATUS_UNRECOGNIZED_TOKEN       = 0x300, // Unrecognized token found
    YAJP_DESERIALIZATION_RESULT_STATUS_ERRNO_SET                = 0x400, // Some internal function of stdlib set errno

    YAJP_DESERIALIZATION_RESULT_STATUS_EXPECTED_OBEGIN          = 0x010, // Parser expected begin of object
    YAJP_DESERIALIZATION_RESULT_STATUS_EXPECTED_OEND            = 0x020, // Parser expected end of object
    YAJP_DESERIALIZATION_RESULT_STATUS_EXPECTED_ABEGIN          = 0x030, // Parser expected begin of array
    YAJP_DESERIALIZATION_RESULT_STATUS_EXPECTED_AEND            = 0x040, // Parser expected end of array
    YAJP_DESERIALIZATION_RESULT_STATUS_EXPECTED_FIELD           = 0x050, // Parser expected JSON field name
    YAJP_DESERIALIZATION_RESULT_STATUS_EXPECTED_VALUE           = 0x060, // Parser expected JSON field value
    YAJP_DESERIALIZATION_RESULT_STATUS_EXPECTED_COMMA           = 0x070, // Parser expected comma
    YAJP_DESERIALIZATION_RESULT_STATUS_EXPECTED_COLON           = 0x080, // Parser expected colon
    YAJP_DESERIALIZATION_RESULT_STATUS_FOUND_OBEGIN             = 0x001, // Parser found begin of object
    YAJP_DESERIALIZATION_RESULT_STATUS_FOUND_OEND               = 0x002, // Parser found end of object
    YAJP_DESERIALIZATION_RESULT_STATUS_FOUND_ABEGIN             = 0x003, // Parser found begin of array
    YAJP_DESERIALIZATION_RESULT_STATUS_FOUND_AEND               = 0x004, // Parser found end of array
    YAJP_DESERIALIZATION_RESULT_STATUS_FOUND_FIELD              = 0x005, // Parser found JSON field name
    YAJP_DESERIALIZATION_RESULT_STATUS_FOUND_VALUE              = 0x006, // Parser found JSON field value
    YAJP_DESERIALIZATION_RESULT_STATUS_FOUND_COMMA              = 0x007, // Parser found comma
    YAJP_DESERIALIZATION_RESULT_STATUS_FOUND_COLON              = 0x008, // Parser found colon
} yajp_deserialization_result_status_t;

/**
 * Store result of deserialization execution.
 *
 * @note Use yajp_deserialization_result_status_to_str to convert execution result to string.
 * @note In case, if yajp_deserialization_result_status_to_str workflow is not suitable, here goes instructions about how
 *       \c status field is initiated. If \c status is equal to YAJP_DESERIALIZATION_RESULT_STATUS_OK than deserialization
 *       succeeded. If \c status is equal to YAJP_DESERIALIZATION_RESULT_STATUS_DESERIALIZATION_ERROR this means, that
 *       function passed as \c setter to \c yajp_deserialization_action_init returned error. If \c status is equal to
 *       YAJP_DESERIALIZATION_RESULT_STATUS_UNRECOGNIZED_TOKEN this means, lexer failed to recognize token in JSON stream.
 *       In other cases \c status shows what unexpected token was found in JSON stream and initialized value should be
 *       split into two parts. Binary multiply \c status to 0x0F0 and compare with
 *       YAJP_DESERIALIZATION_RESULT_STATUS_EXPECTED_<X> to get type of token what parser expects and binary multiply
 *       \c status to 0x00F and compare with YAJP_DESERIALIZATION_RESULT_STATUS_FOUND_<X> to get type of token what
 *       parser has found.
 */
typedef struct yajp_deserialization_result {
        yajp_deserialization_result_status_t status;    // status of deserialization process.
#ifdef YAJP_TRACK_STREAM
        int line_num;                                   // number of line where error happened
        int column_num;                                 // number of collumn where error happened
#endif
    } yajp_deserialization_result_t;

/**
 * Converts result status to string.
 * @param[in]   result    Result of deserialization process
 * @return      String representation of deserialization process result
 *
 * @note Returned value should be freed after using
 */
char *yajp_deserialization_result_status_to_str(const yajp_deserialization_result_t *result);


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
yajp_deserialization_result_t yajp_deserialize_json_stream(FILE *json,
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
yajp_deserialization_result_t yajp_deserialize_json_string(const char *json,
                                 size_t json_size,
                                 const yajp_deserialization_ctx_t *ctx,
                                 void *deserializing_struct,
                                 void *user_data);


/**
 * Generic function to initialize deserialization action like strings and primitives
 *
 * @param[in]   field_name      Name of field in JSON stream
 * @param[in]   name_size       Size of field name in JSON without '\0' character
 * @param[in]   offset          Offset of field in deserializing structure
 * @param[in]   field_size      Size of field in deserializing structure
 * @param[in]   options         Deserialization options for field in deserialization process
 * @param[in]   setter          Pointer to function used to deserialize JSON value into structure field
 * @param[out]  action          Pointer to initializing deserialization action
 *
 * @return  Result of initialization. 0 on success
 *
 * @note    Using this function is not convenient way. Prefer to use macro wrappers.
 */
int yajp_deserialization_action_init(const char *field_name,
                                     size_t name_size,
                                     size_t offset,
                                     size_t field_size,
                                     yajp_deserialization_action_options_t options,
                                     yajp_value_setter_t setter,
                                     yajp_deserialization_action_t *action
                                     );

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
                                           yajp_deserialization_action_t *action
                                           );

/**
 * Generic function to initialize deserialization action for inherited objects
 *
 * @param field_name
 * @param name_size
 * @param field_offset
 * @param field_size
 * @param options
 * @param ctx
 * @param action
 * @return
 */
int yajp_deserialization_object_action_init(const char *field_name,
                                            size_t name_size,
                                            size_t field_offset,
                                            size_t field_size,
                                            yajp_deserialization_action_options_t options,
                                            const yajp_deserialization_ctx_t *ctx,
                                            yajp_deserialization_action_t *action
                                            );

/**
 * *
 * Generic function to initialize deserialization action for arrays of objects
 *
 * @details
 * Array deserialization is a complex process because nigher size of array, nor amount of array dimensions is unknown
 * until deserialization ends. Even if user knows array parameters it's impossible to make it more easier because JSON
 * supports arrays with non-constant amount of elements in each dimensions. To cover this case, YAJP expects that
 * deserializing array will be represented as a struct used to hold dynamic arrays, similar to something like this:
 * @code
 *  typedef struct inner_object inner_object_t;
 *
 *  typedef struct test_struct_arr test_struct_arr_t;
 *  struct test_struct_arr {
 *      union {  // union here can be replaced with struct if you want. Union is used to reduce amount of space
 *          inner_object_t *elems;   // pointer to array used to store deserializing array values
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
 * @param[in]   ctx                 Pointer to deserialization context used to deserialize object
 * @param[out]  action              Pointer to initializing deserialization action
 *
 * @return  Result of initialization. 0 on success
 */
int yajp_deserialization_array_object_action_init(const char *field_name,
                                                  size_t name_size,
                                                  size_t field_offset,
                                                  size_t field_size,
                                                  yajp_deserialization_action_options_t options,
                                                  size_t counter_offset,
                                                  size_t final_dim_offset,
                                                  size_t rows_offset,
                                                  size_t elem_size,
                                                  size_t elems_offset,
                                                  const yajp_deserialization_ctx_t *ctx,
                                                  yajp_deserialization_action_t *action
                                                  );

/**
 * Calculates size of constant string without leading '\0' character.
 *
 * @param[in]   str Constant string
 * @return      Size of string in bytes without '\0' character.
 */
#define string_size_without_null(str) ((sizeof((str))/sizeof(*(str)) - 1) * sizeof(*(str)))

/**
 *  Calculates size of structure member
 */
#define struct_field_size(TYPE, MEMBER) sizeof(((TYPE *)NULL)->MEMBER)

/**
 * Calculates size of object pointed by structure member
 */
#define struct_field_ptr_size(TYPE, MEMBER) sizeof(*((TYPE *)NULL)->MEMBER)

/**
 * Convenient initialization of deserialization action for primitive fields
 *
 * @param[in]   structure   Type of structure where deserializing field is stored
 * @param[in]   field       Name of field in deserializing structure
 * @param[in]   setter      Pointer to function used to deserialize JSON value into structure field
 * @param[out]  action      Pointer to initializing deserialization action
 * @return      Result of initialization. 0 on success
 *
 * @note    Use this macro for deserialization of primitive fields like integers, doubles, booleans. Macro expect that
 *          name of field in deserializaing structure and JSON stream are same.
 *
 * @note    Use YAJP_PRIMITIVE_FIELD_OVERWRITE_DESERIALIZATION_ACTION_INIT to specify name of JSON stream field
 */
#define YAJP_PRIMITIVE_FIELD_DESERIALIZATION_ACTION_INIT(structure, field, setter, action) \
    yajp_deserialization_action_init(                                                      \
        #field,                                                                            \
        string_size_without_null(#field),                                                  \
        offsetof(structure, field),                                                        \
        struct_field_size(structure, field),                                               \
        YAJP_DESERIALIZATION_ACTION_OPTIONS_TYPE_PRIMITIVE,                                \
        setter,                                                                            \
        action)

/*
 * Convenient initialization of deserialization action for primitive fields where JSON field name and deserializing
 * field name are different
 *
 * @param[in]   json_field  Name of field in JSON stream
 * @param[in]   structure   Type of structure where deserializing field is stored
 * @param[in]   field       Name of field in deserializing structure
 * @param[in]   setter      Pointer to function used to deserialize JSON value into structure field
 * @param[out]  action      Pointer to initializing deserialization action
 *
 * @return      Result of initialization. 0 on success
 */
#define YAJP_PRIMITIVE_FIELD_OVERWRITE_DESERIALIZATION_ACTION_INIT(json_field, structure, field, setter, action)    \
    yajp_deserialization_action_init(                                                                               \
        json_field,                                                                                                 \
        string_size_without_null(json_field),                                                                       \
        offsetof(structure, field),                                                                                 \
        struct_field_size(structure, field),                                                                        \
        YAJP_DESERIALIZATION_ACTION_OPTIONS_TYPE_PRIMITIVE,                                                         \
        setter,                                                                                                     \
        action)

/**
 * Convenient initialization of deserialization action for string fields
 *
 * @param[in]   structure
 * @param[in]   field
 * @param[in]   setter
 * @param[in]   allocate
 * @param[out]  action
 *
 * @return
 *
 * @note    Use this macro for deserialization of string fields Macro expect that name of field in deserializing
 *          structure and JSON stream are same.
 */
#define YAJP_STRING_FIELD_DESERIALIZATION_ACTION_INIT(structure, field, setter, allocate, action)                       \
    yajp_deserialization_action_init(                                                                                   \
        #field,                                                                                                         \
        string_size_without_null(#field),                                                                               \
        offsetof(structure, field),                                                                                     \
        struct_field_ptr_size(structure, field),                                                                        \
        (YAJP_DESERIALIZATION_ACTION_OPTIONS_TYPE_STRING |                                                              \
            ((allocate) ? YAJP_DESERIALIZATION_ACTION_OPTIONS_ALLOCATE : YAJP_DESERIALIZATION_ACTION_OPTIONS_NONE)),    \
        setter,                                                                                                         \
        action)

/**
 * Convenient initialization of deserialization action for string fields
 *
 * @param[in]   json_field
 * @param[in]   structure
 * @param[in]   field
 * @param[in]   setter
 * @param[in]   allocate
 * @param[out]  action
 *
 * @return
 *
 * @note    Use this macro for deserialization of string fields Macro expect that name of field in deserializing
 *          structure and JSON stream are same.
 */
#define YAJP_STRING_FIELD_OVERWRITE_DESERIALIZATION_ACTION_INIT(json_field, structure, field, setter, allocate, action) \
    yajp_deserialization_action_init(                                                                                   \
        json_field,                                                                                                     \
        string_size_without_null(json_field),                                                                           \
        offsetof(structure, field),                                                                                     \
        struct_field_ptr_size(structure, field),                                                                        \
        (YAJP_DESERIALIZATION_ACTION_OPTIONS_TYPE_STRING |                                                              \
            ((allocate) ? YAJP_DESERIALIZATION_ACTION_OPTIONS_ALLOCATE : YAJP_DESERIALIZATION_ACTION_OPTIONS_NONE)),    \
        setter,                                                                                                         \
        action)

/**
 * Convenient initialization of deserialization action for array of primitives field
 *
 * @param[in]   structure
 * @param[in]   field
 * @param[in]   field_type
 * @param[in]   counter
 * @param[in]   final_dim
 * @param[in]   rows
 * @param[in]   elems
 * @param[in]   elem_type
 * @param[in]   allocate
 * @param[in]   allocate_elem
 * @param[in]   setter
 * @param[out]  result
 *
 * @note    Use this macro for deserialization of array of primitives field. Macro expects that name of field in
 *          deserializing structure and JSON stream are same.
 */
#define YAJP_ARRAY_OF_PRIMITIVE_FIELD_DESERIALIZATION_ACTION_INIT(structure, field, field_type, counter, final_dim, rows, elems, elem_type, allocate, allocate_elem, setter, action)    \
    yajp_deserialization_array_action_init(                                                                                                                                             \
        #field,                                                                                                                                                                         \
        string_size_without_null(#field),                                                                                                                                               \
        offsetof(structure, field),                                                                                                                                                     \
        sizeof(field_type),                                                                                                                                                             \
        YAJP_DESERIALIZATION_ACTION_OPTIONS_TYPE_ARRAY_OF | YAJP_DESERIALIZATION_ACTION_OPTIONS_TYPE_PRIMITIVE |                                                                        \
            ((allocate) ? YAJP_DESERIALIZATION_ACTION_OPTIONS_ALLOCATE : YAJP_DESERIALIZATION_ACTION_OPTIONS_NONE) |                                                                    \
            ((allocate_elem) ? YAJP_DESERIALIZATION_ACTION_OPTIONS_ALLOCATE_ELEMENTS : YAJP_DESERIALIZATION_ACTION_OPTIONS_NONE),                                                       \
        offsetof(field_type, counter),                                                                                                                                                  \
        offsetof(field_type, final_dim),                                                                                                                                                \
        offsetof(field_type, rows),                                                                                                                                                     \
        sizeof(elem_type),                                                                                                                                                              \
        offsetof(field_type, elems),                                                                                                                                                    \
        setter,                                                                                                                                                                         \
        action                                                                                                                                                                          \
    )

/**
 * Convenient initialization of deserialization action for array of primitives field
 *
 * @param[in]   json_field
 * @param[in]   structure
 * @param[in]   field
 * @param[in]   field_type
 * @param[in]   counter
 * @param[in]   final_dim
 * @param[in]   rows
 * @param[in]   elems
 * @param[in]   elem_type
 * @param[in]   allocate
 * @param[in]   allocate_elem
 * @param[in]   setter
 * @param[out]  result
 *
 * @note    Use this macro for deserialization of array of primitives field. Macro expects that name of field in
 *          deserializing structure and JSON stream are same.
 */
#define YAJP_ARRAY_OF_PRIMITIVE_FIELD_OVERWRITE_DESERIALIZATION_ACTION_INIT(json_field, structure, field, field_type, counter, final_dim, rows, elems, elem_type, allocate, allocate_elem, setter, action)  \
    yajp_deserialization_array_action_init(                                                                                                                                                                 \
        json_field,                                                                                                                                                                                         \
        string_size_without_null(json_field),                                                                                                                                                               \
        offsetof(structure, field),                                                                                                                                                                         \
        sizeof(field_type),                                                                                                                                                                                 \
        YAJP_DESERIALIZATION_ACTION_OPTIONS_TYPE_ARRAY_OF | YAJP_DESERIALIZATION_ACTION_OPTIONS_TYPE_PRIMITIVE |                                                                                            \
            ((allocate) ? YAJP_DESERIALIZATION_ACTION_OPTIONS_ALLOCATE : YAJP_DESERIALIZATION_ACTION_OPTIONS_NONE) |                                                                                        \
            ((allocate_elem) ? YAJP_DESERIALIZATION_ACTION_OPTIONS_ALLOCATE_ELEMENTS : YAJP_DESERIALIZATION_ACTION_OPTIONS_NONE),                                                                           \
        offsetof(field_type, counter),                                                                                                                                                                      \
        offsetof(field_type, final_dim),                                                                                                                                                                    \
        offsetof(field_type, rows),                                                                                                                                                                         \
        sizeof(elem_type),                                                                                                                                                                                  \
        offsetof(field_type, elems),                                                                                                                                                                        \
        setter,                                                                                                                                                                                             \
        action                                                                                                                                                                                              \
    )


/**
 * Convenient initialization of deserialization action for array of primitives field
 *
 * @param[in]   structure
 * @param[in]   field
 * @param[in]   field_type
 * @param[in]   counter
 * @param[in]   final_dim
 * @param[in]   rows
 * @param[in]   elems
 * @param[in]   elem_type
 * @param[in]   allocate
 * @param[in]   allocate_elem
 * @param[in]   setter
 * @param[out]  result
 *
 * @note    Use this macro for deserialization of array of primitives field. Macro expects that name of field in
 *          deserializing structure and JSON stream are same.
 */
#define YAJP_ARRAY_OF_STRING_FIELD_DESERIALIZATION_ACTION_INIT(structure, field, field_type, counter, final_dim, rows, elems, elem_type, allocate, allocate_elem, setter, action)   \
    yajp_deserialization_array_action_init(                                                                                                                                         \
        #field,                                                                                                                                                                     \
        string_size_without_null(#field),                                                                                                                                           \
        offsetof(structure, field),                                                                                                                                                 \
        sizeof(field_type),                                                                                                                                                         \
        YAJP_DESERIALIZATION_ACTION_OPTIONS_TYPE_ARRAY_OF | YAJP_DESERIALIZATION_ACTION_OPTIONS_TYPE_STRING |                                                                       \
            ((allocate) ? YAJP_DESERIALIZATION_ACTION_OPTIONS_ALLOCATE : YAJP_DESERIALIZATION_ACTION_OPTIONS_NONE) |                                                                \
            ((allocate_elem) ? YAJP_DESERIALIZATION_ACTION_OPTIONS_ALLOCATE_ELEMENTS : YAJP_DESERIALIZATION_ACTION_OPTIONS_NONE),                                                   \
        offsetof(field_type, counter),                                                                                                                                              \
        offsetof(field_type, final_dim),                                                                                                                                            \
        offsetof(field_type, rows),                                                                                                                                                 \
        sizeof(elem_type),                                                                                                                                                          \
        offsetof(field_type, elems),                                                                                                                                                \
        setter,                                                                                                                                                                     \
        action                                                                                                                                                                      \
    )

/**
 * Convenient initialization of deserialization action for array of primitives field
 *
 * @param[in]   json_field
 * @param[in]   structure
 * @param[in]   field
 * @param[in]   field_type
 * @param[in]   counter
 * @param[in]   final_dim
 * @param[in]   rows
 * @param[in]   elems
 * @param[in]   elem_type
 * @param[in]   allocate
 * @param[in]   allocate_elem
 * @param[in]   setter
 * @param[out]  result
 *
 * @note    Use this macro for deserialization of array of primitives field. Macro expects that name of field in
 *          deserializing structure and JSON stream are same.
 */
#define YAJP_ARRAY_OF_STRING_FIELD_OVERWRITE_DESERIALIZATION_ACTION_INIT(json_field, structure, field, field_type, counter, final_dim, rows, elems, elem_type, allocate, allocate_elem, setter, action)  \
    yajp_deserialization_array_action_init(                                                                                                                                                              \
        json_field,                                                                                                                                                                                      \
        string_size_without_null(json_field),                                                                                                                                                            \
        offsetof(structure, field),                                                                                                                                                                      \
        sizeof(field_type),                                                                                                                                                                              \
        YAJP_DESERIALIZATION_ACTION_OPTIONS_TYPE_ARRAY_OF | YAJP_DESERIALIZATION_ACTION_OPTIONS_TYPE_STRING |                                                                                            \
            ((allocate) ? YAJP_DESERIALIZATION_ACTION_OPTIONS_ALLOCATE : YAJP_DESERIALIZATION_ACTION_OPTIONS_NONE) |                                                                                     \
            ((allocate_elem) ? YAJP_DESERIALIZATION_ACTION_OPTIONS_ALLOCATE_ELEMENTS : YAJP_DESERIALIZATION_ACTION_OPTIONS_NONE),                                                                        \
        offsetof(field_type, counter),                                                                                                                                                                   \
        offsetof(field_type, final_dim),                                                                                                                                                                 \
        offsetof(field_type, rows),                                                                                                                                                                      \
        sizeof(elem_type),                                                                                                                                                                               \
        offsetof(field_type, elems),                                                                                                                                                                     \
        setter,                                                                                                                                                                                          \
        action                                                                                                                                                                                           \
    )

/**
 * Convenient initialization of deserialization action for object field
 *
 * @param[in]   structure
 * @param[in]   field
 * @param[in]   field_type
 * @param[in]   ctx
 * @param[in]   allocate
 * @param[out]  action
 *
 * @note    Use this macro for deserialization of object field. Macro expects that name of field in
 *          deserializing structure and JSON stream are same.
 */
#define YAJP_OBJECT_FIELD_DESERIALIZATION_ACTION_INIT(structure, field, field_type, ctx, allocate, action)             \
    yajp_deserialization_object_action_init(#field,                                                                    \
        string_size_without_null(#field),                                                                              \
        offsetof(structure, field),                                                                                    \
        sizeof(field_type),                                                                                            \
        YAJP_DESERIALIZATION_ACTION_OPTIONS_TYPE_OBJECT |                                                              \
            ((allocate) ? YAJP_DESERIALIZATION_ACTION_OPTIONS_ALLOCATE : YAJP_DESERIALIZATION_ACTION_OPTIONS_NONE),    \
        ctx,                                                                                                           \
        action                                                                                                         \
    )

/**
 * Convenient initialization of deserialization action for object field
 *
 * @param[in]   json_field
 * @param[in]   structure
 * @param[in]   field
 * @param[in]   ctx
 * @param[in]   allocate
 * @param[out]  action
 *
 */
#define YAJP_OBJECT_FIELD_OVERWRITE_DESERIALIZATION_ACTION_INIT(json_field, structure, field, ctx, allocate, action)

/**
 * Convenient initialization of deserialization action for array of object field
 *
 * @param[in]   structure
 * @param[in]   field
 * @param[in]   field_type
 * @param[in]   counter
 * @param[in]   final_dim
 * @param[in]   rows
 * @param[in]   elems
 * @param[in]   elem_type
 * @param[in]   allocate
 * @param[in]   allocate_elem
 * @param[in]   ctx
 * @param[out]  action
 *
 * @note    Use this macro for deserialization of array of object field. Macro expects that name of field in
 *          deserializing structure and JSON stream are same.
 */
#define YAJP_ARRAY_OF_OBJECT_FIELD_DESERIALIZATION_ACTION_INIT(structure, field, field_type, counter, final_dim, rows, elems, elem_type, allocate, allocate_elem, ctx, action) \
    yajp_deserialization_array_object_action_init(#field,                                                                                                                      \
        string_size_without_null(#field),                                                                                                                                      \
        offsetof(structure, field),                                                                                                                                            \
        sizeof(field_type),                                                                                                                                                    \
        YAJP_DESERIALIZATION_ACTION_OPTIONS_TYPE_ARRAY_OF | YAJP_DESERIALIZATION_ACTION_OPTIONS_TYPE_OBJECT |                                                                  \
            ((allocate) ? YAJP_DESERIALIZATION_ACTION_OPTIONS_ALLOCATE : YAJP_DESERIALIZATION_ACTION_OPTIONS_NONE) |                                                           \
            ((allocate_elem) ? YAJP_DESERIALIZATION_ACTION_OPTIONS_ALLOCATE_ELEMENTS : YAJP_DESERIALIZATION_ACTION_OPTIONS_NONE),                                              \
        offsetof(field_type, counter),                                                                                                                                         \
        offsetof(field_type, final_dim),                                                                                                                                       \
        offsetof(field_type, rows),                                                                                                                                            \
        sizeof(elem_type),                                                                                                                                                     \
        offsetof(field_type, elems),                                                                                                                                           \
        ctx,                                                                                                                                                                   \
        action                                                                                                                                                                 \
    )

/**
 * Convenient initialization of deserialization action for array of object field
 *
 * @param[in]   json_field
 * @param[in]   structure
 * @param[in]   field
 * @param[in]   field_type
 * @param[in]   counter
 * @param[in]   final_dim
 * @param[in]   rows
 * @param[in]   elems
 * @param[in]   elem_type
 * @param[in]   allocate
 * @param[in]   allocate_elem
 * @param[in]   ctx
 * @param[out]  action
 */
#define YAJP_ARRAY_OF_OBJECT_FIELD_OVERWRITE_DESERIALIZATION_ACTION_INIT(json_field, structure, field, field_type, counter, final_dim, rows, elems, elem_type, allocate, allocate_elem, ctx, action)

#endif // YAJP_DESERIALIZE_H