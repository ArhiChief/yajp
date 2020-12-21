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

/**
 *
 */
typedef int (*yajp_value_setter_t)(const uint8_t *name,
                                   size_t name_size,
                                   const uint8_t *value,
                                   size_t value_size,
                                   void *field,
                                   void *user_data
);

/**
 * Type of deserializing field
 */
typedef enum {
    YAJP_DESERIALIZATION_FIELD_TYPE_PRIMITIVE,
    YAJP_DESERIALIZATION_FIELD_TYPE_STRING,
    YAJP_DESERIALIZATION_FIELD_TYPE_PRIMITIVE_ARRAY,
} yajp_deserialization_field_type_t;

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
 * Deserialization action
 */
struct yajp_deserialization_action {
    size_t offset;
    size_t size;
    yajp_deserialization_field_type_t type;

    unsigned long field_key;

    union {
        yajp_value_setter_t setter;
    };
};

/**
 * Represent status of JSON stream deserialization
 */
typedef enum {
    YAJP_DESERIALIZATION_RESULT_STATUS_OK                       = 0x000, // Deserialization succeeded
    YAJP_DESERIALIZATION_RESULT_STATUS_DESERIALIZATION_ERROR    = 0x100, // Failed to deserialize value from JSON stream
    YAJP_DESERIALIZATION_RESULT_STATUS_UNEXPECTED_EOF           = 0x200, // Unexpected end of JSON stream
    YAJP_DESERIALIZATION_RESULT_STATUS_UNRECOGNIZED_TOKEN       = 0x300, // Unrecognized token found

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
typedef struct {
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
 * @param[in]   json        Pointer to JSON stream
 * @param[in]   ctx         Pointer to deserialization context
 * @param[out]  result      Pointer to deserializing structure
 * @param[in]   user_data   Pointer to value what will be passed as \c user_data to \c setter in \c yajp_deserialization_action_init
 * @return      Result of deserialization process. See \c yajp_deserialization_result_t for details
 */
yajp_deserialization_result_t yajp_deserialize_json_stream(FILE *json,
                                 const yajp_deserialization_ctx_t *ctx,
                                 void *result,
                                 void *user_data);

/**
 * Deserialize plain JSON string into provided structure
 * @param[in]   json        Pointer to string with JSON
 * @param[in]   json_size   Size in bytes of deserializing JSON string
 * @param[in]   ctx         Pointer to deserialization context
 * @param[out]  result      Pointer to deserializing structure
 * @param[in]   user_data   Pointer to value what will be passed as \c user_data to \c setter in \c yajp_deserialization_action_init
 * @return      Result of deserialization process. See \c yajp_deserialization_result_t for details
 *
 * @note    This function wraps passed JSON string into \c FILE using \c fmemopen call from \c stdio.h and calls \c yajp_deserialize_json_stream
 */
yajp_deserialization_result_t yajp_deserialize_json_string(const char *json,
                                 size_t json_size,
                                 const yajp_deserialization_ctx_t *ctx,
                                 void *result,
                                 void *user_data);


/**
 * Generic function to initialize deserialization action.
 *
 * @param[in]   field_name      Name of field in JSON stream
 * @param[in]   name_size       Size of field name in JSON without '\0' character
 * @param[in]   offset          Offset of field in deserializing structure
 * @param[in]   field_size      Size of field in deserializing structure
 * @param[in]   field_type      Type of field in deserialization process
 * @param[in]   setter          Pointer to function used to deserialize JSON value into structure field
 * @param[out]  counter_offset  Offset of counter field used to store amount of elements in case of array deserialization
 * @param[out]  result          Pointer to initializing deserialization action
 *
 * @return  Result of initialization. 0 on success
 *
 * @note    Using this function is not convenient way. Prefer to use macro wrappers.
 */
int yajp_deserialization_action_init(const char *field_name,
                                     size_t name_size,
                                     size_t offset,
                                     size_t field_size,
                                     yajp_deserialization_field_type_t field_type,
                                     yajp_value_setter_t setter,
                                     size_t counter_offset,
                                     yajp_deserialization_action_t *result
                                     );

/**
 * Convenient initialization of deserialization action for primitive fields
 *
 * @param[in]   structure   Type of structure where deserializing field is stored
 * @param[in]   field       Name of field in deserializing structure
 * @param[in]   setter      Pointer to function used to deserialize JSON value into structure field
 * @param[out]  result      Pointer to initializing deserialization action
 * @return      Result of initialization. 0 on success
 *
 * @note    Use this macro for deserialization of primitive fields like integers, doubles, booleans. Macro expect that
 *          name of field in deserializaing structure and JSON stream are same.
 *
 * @note    Use YAJP_PRIMITIVE_FIELD_OVERWRITE_DESERIALIZATION_ACTION_INIT to specify name of JSON stream field
 */
#define YAJP_PRIMITIVE_FIELD_DESERIALIZATION_ACTION_INIT(structure, field, setter, result) \
    yajp_deserialization_action_init(                                                      \
        #field,                                                                            \
        str_size_without_null(#field),                                                     \
        offsetof(structure, field),                                                        \
        sizeof(((structure *)NULL)->field),                                                \
        YAJP_DESERIALIZATION_FIELD_TYPE_PRIMITIVE,                                         \
        setter,                                                                            \
        0,                                                                                 \
        result)

/*
 * Convenient initialization of deserialization action for primitive fields where JSON field name and deserializing field name are different
 *
 * @param[in]   json_field  Name of field in JSON stream
 * @param[in]   structure   Type of structure where deserializing field is stored
 * @param[in]   field       Name of field in deserializing structure
 * @param[in]   setter      Pointer to function used to deserialize JSON value into structure field
 * @param[out]  result      Pointer to initializing deserialization action
 * @return      Result of initialization. 0 on success
 */
#define YAJP_PRIMITIVE_FIELD_OVERWRITE_DESERIALIZATION_ACTION_INIT(json_field, structure, field, setter, result)    \
    yajp_deserialization_action_init(                                                                               \
        json_field,                                                                                                 \
        str_size_without_null(json_field),                                                                          \
        offsetof(structure, field),                                                                                 \
        sizeof(((structure *)NULL)->field),                                                                         \
        YAJP_DESERIALIZATION_FIELD_TYPE_PRIMITIVE,                                                                  \
        setter,                                                                                                     \
        0,                                                                                                          \
        result)

/**
 * Convenient initialization of deserialization action for primitive fields
 *
 * @param[in]   structure   Type of structure where deserializing field is stored
 * @param[in]   field       Name of field in deserializing structure
 * @param[in]   setter      Pointer to function used to deserialize JSON value into structure field
 * @param[out]  result      Pointer to initializing deserialization action
 * @return      Result of initialization. 0 on success
 *
 * @note    Use this macro for deserialization of primitive fields like integers, doubles, booleans. Macro expect that
 *          name of field in deserializaing structure and JSON stream are same.
 *
 * @note    Use YAJP_PRIMITIVE_FIELD_OVERWRITE_DESERIALIZATION_ACTION_INIT to specify name of JSON stream field
 */
#define YAJP_PRIMITIVE_FIELD_DESERIALIZATION_ACTION_INIT(structure, field, setter, result) \
    yajp_deserialization_action_init(                                                      \
        #field,                                                                            \
        str_size_without_null(#field),                                                     \
        offsetof(structure, field),                                                        \
        sizeof(((structure *)NULL)->field),                                                \
        YAJP_DESERIALIZATION_FIELD_TYPE_PRIMITIVE,                                         \
        setter,                                                                            \
        0,                                                                                 \
        result)

/*
 * Convenient initialization of deserialization action for primitive fields where JSON field name and deserializing field name are different
 *
 * @param[in]   json_field  Name of field in JSON stream
 * @param[in]   structure   Type of structure where deserializing field is stored
 * @param[in]   field       Name of field in deserializing structure
 * @param[in]   setter      Pointer to function used to deserialize JSON value into structure field
 * @param[out]  result      Pointer to initializing deserialization action
 * @return      Result of initialization. 0 on success
 */
#define YAJP_PRIMITIVE_FIELD_OVERWRITE_DESERIALIZATION_ACTION_INIT(json_field, structure, field, setter, result)    \
    yajp_deserialization_action_init(                                                                               \
        json_field,                                                                                                 \
        str_size_without_null(json_field),                                                                          \
        offsetof(structure, field),                                                                                 \
        sizeof(((structure *)NULL)->field),                                                                         \
        YAJP_DESERIALIZATION_FIELD_TYPE_PRIMITIVE,                                                                  \
        setter,                                                                                                     \
        0,                                                                                                          \
        result)

#endif //YAJP_DESERIALIZE_H
