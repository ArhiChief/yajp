/* -*- Mode: C; indent-tabs-mode: t; c-basic-offset: 4; tab-width: 4 -*-  */
/*
 * deserialization_action_initialization.h
 * Copyright (C) 2021 Sergei Kosivchenko <arhichief@gmail.com> 
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

#include <yajp/deserialization.h>

do {
#define YAJP_DESERIALIZATION_GET_FIELD_NAME_SIZE(str)   ((sizeof((str)) / sizeof(*(str)) - 1) * sizeof(*(str)))
#define YAJP_DESERIALIZATION_STRINGIFY2(str)            #str
#define YAJP_DESERIALIZATION_STRINGIFY(str)             YAJP_DESERIALIZATION_STRINGIFY2(str)

//#define YAJP_DESERIALIZATION_OPTIONS_TYPE_NUMBER        0b00000001  // 0b 00 0 0 0001
//#define YAJP_DESERIALIZATION_OPTIONS_TYPE_STRING        0b00000010  // 0b 00 0 0 0010
//#define YAJP_DESERIALIZATION_OPTIONS_TYPE_BOOLEAN       0b00000100  // 0b 00 0 0 0100
//#define YAJP_DESERIALIZATION_OPTIONS_TYPE_OBJECT        0b00001000  // 0b 00 0 0 1000
//#define YAJP_DESERIALIZATION_TYPE_ARRAY_OF              0b00010000  // 0b 00 0 1 0000
//#define YAJP_DESERIALIZATION_TYPE_NULLABLE              0b00100000  // 0b 00 1 0 0000
//#define YAJP_DESERIALIZATION_OPTIONS_ALLOCATE           0b01000000  // 0b 01 0 0 0000
//#define YAJP_DESERIALIZATION_OPTIONS_ALLOCATE_ELEMENTS  0b10000000  // 0b 10 0 0 0000


//#define YAJP_DESERIALIZATION_ACTION                     &actions[0]
//#define YAJP_DESERIALIZATION_ACTION_INIT_RESULT         res
//
//#define YAJP_DESERIALIZATION_FIELD_NAME                 "field1"
//#define YAJP_DESERIALIZATION_FIELD_NAME_SIZE            FIELD_NAME_SIZE("field1")
//#define YAJP_DESERIALIZATION_FIELD_TYPE                 (YAJP_DESERIALIZATION_OPTIONS_TYPE_OBJECT |  YAJP_DESERIALIZATION_TYPE_ARRAY_OF | YAJP_DESERIALIZATION_TYPE_NULLABLE)
//#define YAJP_DESERIALIZATION_OPTIONS                    (YAJP_DESERIALIZATION_OPTIONS_ALLOCATE | YAJP_DESERIALIZATION_OPTIONS_ALLOCATE_ELEMENTS)
//
//#define YAJP_DESERIALIZATION_STRUCT_FIELD_HOLDER_TYPE   test_struct_t
//#define YAJP_DESERIALIZATION_STRUCT_FIELD_NAME          my_field
//
//#define YAJP_DESERIALIZATION_SETTER                     yajp_set_int
//
//#define YAJP_DESERIALIZATION_OBJECT_CONTEXT             my_deser_ctx
//
//#define YAJP_DESERIALIZATION_ARRAY_ELEMENT_TYPE         int8_t
//#define YAJP_DESERIALIZATION_ARRAY_ELEMENTS             elems
//#define YAJP_DESERIALIZATION_ARRAY_ROWS                 rows
//#define YAJP_DESERIALIZATION_ARRAY_COUNTER              counter
//#define YAJP_DESERIALIZATION_ARRAY_FINAL_DIM            final_dim


// definations checks and default values setting
#ifndef YAJP_DESERIALIZATION_ACTION
    #error "YAJP_DESERIALIZATION_ACTION is not defined"
#endif

#ifndef YAJP_DESERIALIZATION_STRUCT_FIELD_HOLDER_TYPE
    #error "YAJP_DESERIALIZATION_STRUCT_FIELD_HOLDER_TYPE is not defined"
#endif

#ifndef YAJP_DESERIALIZATION_STRUCT_FIELD_NAME
    #error "YAJP_DESERIALIZATION_STRUCT_FIELD_NAME is not defined"
#endif

#if (!defined(YAJP_DESERIALIZATION_FIELD_TYPE)) || !(YAJP_DESERIALIZATION_FIELD_TYPE & 0b00001111)
    #error "YAJP_DESERIALIZATION_FIELD_TYPE is not defined or doesn't specify field type"
#endif

#if ((YAJP_DESERIALIZATION_FIELD_TYPE & 0b00001111) & ((YAJP_DESERIALIZATION_FIELD_TYPE & 0b00001111) - 1)) // n & (n - 1) != 0 checks whenever more than one bit is set
    #error "YAJP_DESERIALIZATION_FIELD_TYPE can't describe more than one type"
#endif

#if (YAJP_DESERIALIZATION_FIELD_TYPE & YAJP_DESERIALIZATION_TYPE_OBJECT)
    #if !defined(YAJP_DESERIALIZATION_OBJECT_CONTEXT)
        #error "YAJP_DESERIALIZATION_OBJECT_CONTEXT is not defined"
    #endif
#else
    #if !defined(YAJP_DESERIALIZATION_SETTER)
        #error "YAJP_DESERIALIZATION_SETTER is not defined"
    #endif
#endif

#if (YAJP_DESERIALIZATION_FIELD_TYPE & YAJP_DESERIALIZATION_TYPE_ARRAY_OF)
    #if !defined(YAJP_DESERIALIZATION_ARRAY_ELEMENT_TYPE)
        #error "YAJP_DESERIALIZATION_ARRAY_ELEMENT_TYPE is not defined"
    #elif !defined(YAJP_DESERIALIZATION_ARRAY_ELEMENTS)
        #error "YAJP_DESERIALIZATION_ARRAY_ELEMENTS is not defined"
    #elif !defined(YAJP_DESERIALIZATION_ARRAY_ROWS)
        #error "YAJP_DESERIALIZATION_ARRAY_ROWS is not defined"
    #elif !defined(YAJP_DESERIALIZATION_ARRAY_COUNTER)
        #error "YAJP_DESERIALIZATION_ARRAY_COUNTER is not defined"
    #elif !defined(YAJP_DESERIALIZATION_ARRAY_FINAL_DIM)
        #error "YAJP_DESERIALIZATION_ARRAY_FINAL_DIM is not defined"
    #endif
#endif


#ifndef YAJP_DESERIALIZATION_OPTIONS
    #define YAJP_DESERIALIZATION_OPTIONS 0b00000000
#endif

#ifndef YAJP_DESERIALIZATION_FIELD_NAME
    #define YAJP_DESERIALIZATION_FIELD_NAME YAJP_DESERIALIZATION_STRINGIFY(YAJP_DESERIALIZATION_STRUCT_FIELD_NAME)
#endif

#ifndef YAJP_DESERIALIZATION_FIELD_NAME_SIZE
    #define YAJP_DESERIALIZATION_FIELD_NAME_SIZE YAJP_DESERIALIZATION_GET_FIELD_NAME_SIZE(YAJP_DESERIALIZATION_FIELD_NAME)
#endif

#ifndef YAJP_DESERIALIZATION_SETTER
    #define YAJP_DESERIALIZATION_SETTER NULL
#endif

#ifndef YAJP_DESERIALIZATION_OBJECT_CONTEXT
    #define YAJP_DESERIALIZATION_OBJECT_CONTEXT NULL
#endif

#define YAJP_DESERIALIZATION_ACTION_INIT_FIELD_OFFSET   offsetof(YAJP_DESERIALIZATION_STRUCT_FIELD_HOLDER_TYPE, YAJP_DESERIALIZATION_STRUCT_FIELD_NAME)

#if (YAJP_DESERIALIZATION_OPTIONS & YAJP_DESERIALIZATION_OPTIONS_ALLOCATE) || (YAJP_DESERIALIZATION_FIELD_TYPE & (YAJP_DESERIALIZATION_TYPE_NULLABLE | YAJP_DESERIALIZATION_TYPE_STRING))
    // field of struct expected to be a pointer
    #define YAJP_DESERIALIZATION_ACTION_INIT_FIELD_SIZE sizeof(*((YAJP_DESERIALIZATION_STRUCT_FIELD_HOLDER_TYPE *)NULL)->YAJP_DESERIALIZATION_STRUCT_FIELD_NAME)
    #define YAJP_DESERIALIZATION_ACTION_INIT_FIELD_TYPE typeof(*((YAJP_DESERIALIZATION_STRUCT_FIELD_HOLDER_TYPE *)NULL)->YAJP_DESERIALIZATION_STRUCT_FIELD_NAME)
#else
    #define YAJP_DESERIALIZATION_ACTION_INIT_FIELD_SIZE sizeof(((YAJP_DESERIALIZATION_STRUCT_FIELD_HOLDER_TYPE *)NULL)->YAJP_DESERIALIZATION_STRUCT_FIELD_NAME)
    #define YAJP_DESERIALIZATION_ACTION_INIT_FIELD_TYPE typeof(((YAJP_DESERIALIZATION_STRUCT_FIELD_HOLDER_TYPE *)NULL)->YAJP_DESERIALIZATION_STRUCT_FIELD_NAME)
#endif

#if (YAJP_DESERIALIZATION_FIELD_TYPE & YAJP_DESERIALIZATION_TYPE_ARRAY_OF)
    #define YAJP_DESERIALIZATION_ACTION_INIT_COUNTER_OFFSET     offsetof(YAJP_DESERIALIZATION_ACTION_INIT_FIELD_TYPE, YAJP_DESERIALIZATION_ARRAY_COUNTER)
    #define YAJP_DESERIALIZATION_ACTION_INIT_FINAL_DIM_OFFSET   offsetof(YAJP_DESERIALIZATION_ACTION_INIT_FIELD_TYPE, YAJP_DESERIALIZATION_ARRAY_FINAL_DIM)
    #define YAJP_DESERIALIZATION_ACTION_INIT_ROWS_OFFSET        offsetof(YAJP_DESERIALIZATION_ACTION_INIT_FIELD_TYPE, YAJP_DESERIALIZATION_ARRAY_ROWS)
    #define YAJP_DESERIALIZATION_ACTION_INIT_ELEMS_OFFSET       offsetof(YAJP_DESERIALIZATION_ACTION_INIT_FIELD_TYPE, YAJP_DESERIALIZATION_ARRAY_ELEMENTS)
    #define YAJP_DESERIALIZATION_ACTION_INIT_ELEM_SIZE          sizeof(YAJP_DESERIALIZATION_ARRAY_ELEMENT_TYPE)
#else
    #define YAJP_DESERIALIZATION_ACTION_INIT_COUNTER_OFFSET     0
    #define YAJP_DESERIALIZATION_ACTION_INIT_FINAL_DIM_OFFSET   0
    #define YAJP_DESERIALIZATION_ACTION_INIT_ROWS_OFFSET        0
    #define YAJP_DESERIALIZATION_ACTION_INIT_ELEMS_OFFSET       0

    #if (YAJP_DESERIALIZATION_FIELD_TYPE & YAJP_DESERIALIZATION_TYPE_STRING)
        #define YAJP_DESERIALIZATION_ACTION_INIT_ELEM_SIZE      sizeof(YAJP_DESERIALIZATION_ACTION_INIT_FIELD_TYPE)
    #else
        #define YAJP_DESERIALIZATION_ACTION_INIT_ELEM_SIZE      0
    #endif
#endif

#ifdef YAJP_DESERIALIZATION_ACTION_INIT_RESULT
    YAJP_DESERIALIZATION_ACTION_INIT_RESULT = yajp_deserialization_action_init(
            YAJP_DESERIALIZATION_FIELD_NAME,                                    // name
            YAJP_DESERIALIZATION_FIELD_NAME_SIZE,                               // name_size
            YAJP_DESERIALIZATION_ACTION_INIT_FIELD_OFFSET,                      // field_offset
            YAJP_DESERIALIZATION_ACTION_INIT_FIELD_SIZE,                        // field_size
            (YAJP_DESERIALIZATION_FIELD_TYPE | YAJP_DESERIALIZATION_OPTIONS),   // options
            YAJP_DESERIALIZATION_ACTION_INIT_COUNTER_OFFSET,                    // counter_offset
            YAJP_DESERIALIZATION_ACTION_INIT_FINAL_DIM_OFFSET,                  // final_dim_offset
            YAJP_DESERIALIZATION_ACTION_INIT_ROWS_OFFSET,                       // rows_offset
            YAJP_DESERIALIZATION_ACTION_INIT_ELEMS_OFFSET,                      // elems_offset
            YAJP_DESERIALIZATION_ACTION_INIT_ELEM_SIZE,                         // elem_size
            YAJP_DESERIALIZATION_SETTER,                                        // setter
            YAJP_DESERIALIZATION_OBJECT_CONTEXT,                                // ctx
            YAJP_DESERIALIZATION_ACTION                                         // result
        );
#else
    yajp_deserialization_action_init(
        YAJP_DESERIALIZATION_FIELD_NAME,                                        // name
        YAJP_DESERIALIZATION_FIELD_NAME_SIZE,                                   // name_size
        YAJP_DESERIALIZATION_ACTION_INIT_FIELD_OFFSET,                          // field_offset
        YAJP_DESERIALIZATION_ACTION_INIT_FIELD_SIZE,                            // field_size
        (YAJP_DESERIALIZATION_FIELD_TYPE | YAJP_DESERIALIZATION_OPTIONS),       // options
        YAJP_DESERIALIZATION_ACTION_INIT_COUNTER_OFFSET,                        // counter_offset
        YAJP_DESERIALIZATION_ACTION_INIT_FINAL_DIM_OFFSET,                      // final_dim_offset
        YAJP_DESERIALIZATION_ACTION_INIT_ROWS_OFFSET,                           // rows_offset
        YAJP_DESERIALIZATION_ACTION_INIT_ELEMS_OFFSET,                          // elems_offset
        YAJP_DESERIALIZATION_ACTION_INIT_ELEM_SIZE,                             // elem_size
        YAJP_DESERIALIZATION_SETTER,                                            // setter
        YAJP_DESERIALIZATION_OBJECT_CONTEXT,                                    // ctx
        YAJP_DESERIALIZATION_ACTION                                             // result
    );
#endif

#undef YAJP_DESERIALIZATION_GET_FIELD_NAME_SIZE
#undef YAJP_DESERIALIZATION_STRINGIFY2
#undef YAJP_DESERIALIZATION_STRINGIFY
#undef YAJP_DESERIALIZATION_ACTION
#undef YAJP_DESERIALIZATION_ACTION_INIT_RESULT
#undef YAJP_DESERIALIZATION_FIELD_NAME
#undef YAJP_DESERIALIZATION_FIELD_NAME_SIZE
#undef YAJP_DESERIALIZATION_FIELD_TYPE
#undef YAJP_DESERIALIZATION_OPTIONS
#undef YAJP_DESERIALIZATION_STRUCT_FIELD_HOLDER_TYPE
#undef YAJP_DESERIALIZATION_STRUCT_FIELD_NAME
#undef YAJP_DESERIALIZATION_SETTER
#undef YAJP_DESERIALIZATION_OBJECT_CONTEXT
#undef YAJP_DESERIALIZATION_ARRAY_ELEMENT_TYPE
#undef YAJP_DESERIALIZATION_ARRAY_ELEMENTS
#undef YAJP_DESERIALIZATION_ARRAY_ROWS
#undef YAJP_DESERIALIZATION_ARRAY_COUNTER
#undef YAJP_DESERIALIZATION_ARRAY_FINAL_DIM
#undef YAJP_DESERIALIZATION_OPTIONS
#undef YAJP_DESERIALIZATION_FIELD_NAME
#undef YAJP_DESERIALIZATION_SETTER
#undef YAJP_DESERIALIZATION_OBJECT_CONTEXT
#undef YAJP_DESERIALIZATION_ACTION_INIT_FIELD_OFFSET
#undef YAJP_DESERIALIZATION_ACTION_INIT_FIELD_SIZE
#undef YAJP_DESERIALIZATION_ACTION_INIT_FIELD_TYPE
#undef YAJP_DESERIALIZATION_ACTION_INIT_COUNTER_OFFSET
#undef YAJP_DESERIALIZATION_ACTION_INIT_FINAL_DIM_OFFSET
#undef YAJP_DESERIALIZATION_ACTION_INIT_ROWS_OFFSET
#undef YAJP_DESERIALIZATION_ACTION_INIT_ELEMS_OFFSET
#undef YAJP_DESERIALIZATION_ACTION_INIT_ELEM_SIZE
} while(0);