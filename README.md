# YAJP - Yet Another JSON Processor

**Yet Another JSON Processor** or **yajp** is a C library to serialize and deserialize JSON streams. Just like 
[json-c](https://github.com/json-c/json-c) and others implementations **yajp** provides interfaces to user programs to
deserialize JSON stream into C structures and serialize C structures into JSON stream. It's based on 
[re2c](https://re2c.org/index.html) lexer and [lemon](https://www.hwaci.com/sw/lemon/) LALR(1) parser generators.

But with some exceptions:
- Libraries like `json-c` process JSON by converting it to object trees and after what user should traverse this trees 
  manually to get data. `yajp` do it another way providing special interface to user what will used to describe
  how JSON stream should be deserialized into provided structure; 
- `yajp` supports working with streams using `FILE` from `stdio.h`. This means what `yajp` don't need whole json stream.
  In some situations this can help to save memory. `yajp` also support working with plain JSON string by converting 
  them to `FILE` using call to `fmemopen` function;
- It's fully MT-safe. Object used to describe serialization and deserialization can be instantiated and initiated
  once and used multiple times and in multiple threads;
  
## Building and installing
### Prerequisites:
- `gcc`, `cland` or other C compiler what supports C11;
- `cmake >= 3.18`;

To generate documentations you will also need:
- `doxygen>=1.8.13`

### Building
Navigate to directory with **YAJP** sources and execute:

```shell
cmake -S . -B ./yajp-cmake-build
cmake --build ./yajp-cmake-build
```
### Installing
```shell
cmake --install ./yajp-cmake-build
```

### Building documentation
TBD

### CMake options

| Variable               | Type   | Default value  | Description                                                                                                                                                                              |
|------------------------|--------|----------------|------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------------|
| YAJP_GENERATE_LEXER    | BOOL   | OFF            | Force CMake to generate new lexer. `re2c` should be installed                                                                                                                            |
| YAJP_GENERATE_PARSER   | BOOL   | OFF            | Force CMake to generate new parser. `lemon` should be installed                                                                                                                          |
| YAJP_TRACK_STREAM      | BOOL   | ON             | Track parsing stream. In case of error, line and column number with error will be returned. Not implemented for now.                                                                    |
| YAJP_BUFFER_SIZE       | STRING | 32             | Size in bytes of buffers used to work with JSON. If value can't be fitted into buffer it will be extended enough to handle value and size will be multiplicands by **YAJP_BUFFER_SIZE**. | 

## Usage

**YAJP** can be added as dependency to your CMake project as a subdirectory or as package obtained by `find-package`:
 - If **YAJP** is added to your project as a subproject:
```cmake
include(<path/to/yajp/dir>)
```
 - If yajp is installed to your sysroot then `find-package` command can be used:
```cmake
find_package(yajp REQUIRED)
```

After that, in can be linked with your targets like this:
```cmake
target_link_libraries(target_name PRIVATE yajp::yajp_lib)
```

## Deserialization
Deserialization of JSON stream directly into structure can be achieved by by declaring deserialization rules for each 
deserializing field in JSON stream. Fields of JSON who do not have corresponding deserialization rule will be skipped.
Deserialization context is used to hold group of deserialization actions and bind them with to corresponding type. 

All required structure and functions for deserialization are defined in `yajp/deserialization.h` header file.   

`struct yajp_deserialization_rule`(typedef version `yajp_deserialization_rule_t`) is used to store deserialization rule 
for type field. Rule can be instantiated once and used multiple times and it is MT-Safe. Next function is used to
initialize deserialization rule:
```c
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
                                   yajp_deserialization_rule_t *result);
```
- `name`              - Name of field in JSON stream;
- `name_size`         - Size of field in JSON stream without '\0' in bytes;
- `field_offset`      - Offset of deserializing field in deserializing structure in bytes;
- `field_size`        - Size of deserializing field in deserializing structure in bytes;
- `options`           - Deserialization action options;
- `counter_offset`    - Offset of ***counter*** field inside array holder structure in bytes;
- `final_dim_offset`  - Offset of ***final dimension flag*** field inside array holder structure in bytes;
- `rows_offset`       - Offset of ***rows*** field inside array holder structure in bytes;
- `elems_offset`      - Offset of ***elements*** field inside array holder structure in bytes;
- `elem_size`         - Size of array element or string character;
- `setter`            - Pointer to function used to deserialize JSON and init appropriate structure field;
- `ctx`               - Pointer to deserialization context used to deserialize inherit object;
- `result`            - Pointer to resulting deserialization rule.

`yajp_deserialization_rule_init()` return `0` on success or one of  next error codes:
- `-1` - error. // TBD

`yajp_deserialization_rule_init()` is generic function and should be used in rare cases. Consider using [Declarative API](#sec-declarative_api) 
to declare deserialization rules.

`struct yajp_deserialization_rule` (typedef version `yajp_deserialization_rule_t`) is used as deserialization context. 
Deserialization context is used to hold set of deserialization actions for type. Deserialization context just like a 
deserialization rule can be instantiated once and used multiple times in multiple threads. Next function is used to 
initialize deserialization context:

```c
int yajp_deserialization_context_init(yajp_deserialization_rule_t *acts, 
                                      int count, 
                                      yajp_deserialization_context_t *ctx);
```
- `acts`  - Pointer to array of deserialization action;
- `count` - Number of deserialization action in array;
- `ctx`   - Pointer to initializing deserialization context.

Result of deserialization context initialization. `0` on success.

Next two functions are used to deserialize JSON stream or string:
```c
int yajp_deserialize_json_stream(FILE *json,
                                 const yajp_deserialization_context_t *ctx,
                                 void *deserializing_struct,
                                 void *user_data);
```
- `json`                  - Pointer to JSON stream;
- `ctx`                   - Pointer to deserialization context;
- `deserializing_struct`  - Pointer to deserializing structure;
- `user_data`             - Pointer to value what will be passed as **user_data** to **setter** in `yajp_deserialization_rule_init()`.

Result of deserialization process. `0` - on success.

```c
int yajp_deserialize_json_string(const char *json,
                                 size_t json_size,
                                 const yajp_deserialization_context_t *ctx,
                                 void *deserializing_struct,
                                 void *user_data);
```
- `json`                  - Pointer to string with JSON;
- `json_size`             - Size in bytes of deserializing JSON string;
- `ctx`                   - Pointer to deserialization context;
- `deserializing_struct`  - Pointer to deserializing structure;
- `user_data`             - Pointer to value what will be passed as **user_data** to **setter** in `yajp_deserialization_rule_init()`.

Result of deserialization process. `0` - on success.

`int yajp_deserialize_json_string()` wraps passed JSON string into `FILE` using `fmemopen()` call from `stdio.h`.

###<a id="sec-declarative_api"></a>Declaratice API
Declarative API is a convenient way to initialize deserialization rules. Each declaration can be divided into three parts:
description of deserializing field in JSON stream, declaration of result handling field and deserialization options. 
At the end of each declarations `#include <yajp/deserialization_action_initialization.h>` should be added. 
`deserialization_action_initialization.h` contains multiple checks and calculations using C - preprocessor macros and 
based on provided declarations calls and provide parameters for `yajp_deserialization_rule_init()`. 

Some of declarations are mandatory, some declaration should be declared according to content of mandatory declarations 
and others can be optional.

| Declaration name                                  | Possible value                                                                                    | Required                                                                              | Description                                                                                                                                           |
|---------------------------------------------------|---------------------------------------------------------------------------------------------------|---------------------------------------------------------------------------------------|-------------------------------------------------------------------------------------------------------------------------------------------------------|
| **YAJP_DESERIALIZATION_RULE**                     | Pointer to instance of `yajp_deserialization_rule_t`                                              | Yes                                                                                   | Declare pointer to the action what will store rule declaration                                                                                        |
| **YAJP_DESERIALIZATION_RULE_INIT_RESULT**         | Name of variable to hold return value of `yajp_deserialization_rule_init()`                       | Optional                                                                              | Declare name of variable what will be used to hold return result of rule initialization function                                                      |
| **YAJP_DESERIALIZATION_FIELD_NAME**               | String with name of field in JSON stream                                                          | Optional                                                                              | Used to declare name of field in JSON. If not specified, value declared in **YAJP_DESERIALIZATION_STRUCT_FIELD_NAME** will be used                    |
| **YAJP_DESERIALIZATION_FIELD_NAME_SIZE**          | Size of String with name of field in JSON stream without `\0` symbol                              | Optional                                                                              | Declares size in bytes of JSON field name string without `\0`. If not specified will be calculated from value in **YAJP_DESERIALIZATION_FIELD_NAME**  |
| **YAJP_DESERIALIZATION_FIELD_TYPE**               | See [Field types](#sec-field_types)                                                               | Yes                                                                                   | Specifies type of field in JSON stream                                                                                                                |
| **YAJP_DESERIALIZATION_OPTIONS**                  | See [Deserialization options](#sec-deserialization_options)                                       | Optional                                                                              | Specifies some options of deserializing value from JSON to handling field                                                                             |
| **YAJP_DESERIALIZATION_STRUCT_FIELD_HOLDER_TYPE** | Type of structure what hold deserializing value                                                   | Yes                                                                                   | Name of type what contains field used to hold JSON value                                                                                              |
| **YAJP_DESERIALIZATION_STRUCT_FIELD_NAME**        | Name of field what hold deserializing value                                                       | Yes                                                                                   | Name of the field what will store deserializing value                                                                                                 |
| **YAJP_DESERIALIZATION_SETTER**                   | Pointer to `yajp_value_setter_t`                                                                  | Yes, in case if **YAJP_DESERIALIZATION_FIELD_TYPE** is not object or array of objects | Specify function what will be used as value setter (convert and set JSON value to type of field in C struct)                                          |
| **YAJP_DESERIALIZATION_OBJECT_CONTEXT**           | Pointer to `yajp_deserialization_context_t`                                                       | Yes, in case if **YAJP_DESERIALIZATION_FIELD_TYPE** is object or array of objects     | Specify deserialization context what will be used to deserialize JSON field and set values in C struct                                                |
| **YAJP_DESERIALIZATION_ARRAY_ELEMENT_TYPE**       | Type of array element. See [Array deserialization](#sec-array_deserialization)                     | Yes, in case if **YAJP_DESERIALIZATION_FIELD_TYPE** is array                          | Name of type of array element                                                                                                                         |
| **YAJP_DESERIALIZATION_ARRAY_ELEMENTS**           | Name of ***element*** field. See [Array deserialization](#sec-array_deserialization)               | Yes, in case if **YAJP_DESERIALIZATION_FIELD_TYPE** is array                          | Name of field in array holding structure which is used to store array elements                                                                        |
| **YAJP_DESERIALIZATION_ARRAY_ROWS**               | Name of ***rows*** field. See [Array deserialization](#sec-array_deserialization)                  | Yes, in case if **YAJP_DESERIALIZATION_FIELD_TYPE** is array                          | Name of field in array holding structure which is used to store sub-arrays                                                                            |
| **YAJP_DESERIALIZATION_ARRAY_COUNTER**            | Name of ***counter*** field. See [Array deserialization](#sec-array_deserialization)               | Yes, in case if **YAJP_DESERIALIZATION_FIELD_TYPE** is array                          | Name of field in array holding structure which is used to count objects in ***rows***/***elements***                                                  |
| **YAJP_DESERIALIZATION_ARRAY_FINAL_DIM**          | Name of ***final dimension flag*** field. See [Array deserialization](#sec-array_deserialization)  | Yes, in case if **YAJP_DESERIALIZATION_FIELD_TYPE** is array                          | Name of field in array holding structure which is used to specify what current structure holds elements or sub-arrays                                 |

So, if we want to declare deserialization rule for processing ***number*** named ***other_field*** in JSON and store it
in ***short_field*** of ***test_struct_t*** we need to write something like this:
```c
typedef struct {
    short short_field;
} test_struct_t;

int ret;
yajp_deserialization_rule_t actions;

// out JSON will look like this: { "other_field": 24 }

// declare rules for test_struct_t.short_field
#define YAJP_DESERIALIZATION_FIELD_NAME                 "other_field"
#define YAJP_DESERIALIZATION_STRUCT_FIELD_HOLDER_TYPE   test_struct_t
#define YAJP_DESERIALIZATION_STRUCT_FIELD_NAME          short_field
#define YAJP_DESERIALIZATION_FIELD_TYPE                 (YAJP_DESERIALIZATION_TYPE_NUMBER)
#define YAJP_DESERIALIZATION_SETTER                     yajp_set_short
#define YAJP_DESERIALIZATION_RULE                       &actions[0]
#define YAJP_DESERIALIZATION_RULE_INIT_RESULT           ret

#include <yajp/deserialization_action_initialization.h>
if (ret) {
    perror("Failed to initialize action");
}
// ==========================================
```

#### <a id="sec-field_types"></a>Field types
**YAJP_DESERIALIZATION_FIELD_TYPE** can be presented as binary disjunction combination of type and type extension(s):

| Type Name                             | Description                             |
|---------------------------------------|-----------------------------------------|
| **YAJP_DESERIALIZATION_TYPE_NUMBER**  | Json field is number (integer or real)  |
| **YAJP_DESERIALIZATION_TYPE_STRING**  | Json field is string                    |
| **YAJP_DESERIALIZATION_TYPE_BOOLEAN** | Json field is boolean                   |
| **YAJP_DESERIALIZATION_TYPE_OBJECT**  | Json field is object                    |

| Type Extension Name                     | Description                                         | 
|-----------------------------------------|-----------------------------------------------------|
| **YAJP_DESERIALIZATION_TYPE_ARRAY_OF**  | JSON filed is array of some type (string or object) |
| **YAJP_DESERIALIZATION_TYPE_NULLABLE**  | JSON filed can be NULL                              |

**YAJP** expect that field type can be combination of one of types and one or more type extensions.

Allowed combination:
- `YAJP_DESERIALIZATION_TYPE_STRING`;
- `YAJP_DESERIALIZATION_TYPE_NUMBER | YAJP_DESERIALIZATION_TYPE_NULLABLE`;
- `YAJP_DESERIALIZATION_TYPE_NULLABLE | YAJP_DESERIALIZATION_TYPE_ARRAY_OF | YAJP_DESERIALIZATION_TYPE_OBJECT`.

Not allowed combination:
- `YAJP_DESERIALIZATION_TYPE_STRING | YAJP_DESERIALIZATION_TYPE_BOOLEAN`;
- `YAJP_DESERIALIZATION_TYPE_NUMBER | YAJP_DESERIALIZATION_TYPE_OBJECT | YAJP_DESERIALIZATION_TYPE_NULLABLE`;
- `YAJP_DESERIALIZATION_TYPE_NULLABLE | YAJP_DESERIALIZATION_TYPE_ARRAY_OF | YAJP_DESERIALIZATION_TYPE_BOOLEAN | YAJP_DESERIALIZATION_TYPE_NUMBER | YAJP_DESERIALIZATION_TYPE_OBJECT`.

To prevent possible errors it's recommended to wrap field type declaration into bracket:
```c
#define YAJP_DESERIALIZATION_FIELD_TYPE     (YAJP_DESERIALIZATION_TYPE_ARRAY_OF | YAJP_DESERIALIZATION_TYPE_OBJECT)
```

#### <a id="sec-deserialization_options"></a> Deserialization options
Deserialization options are used to tell **YAJP** how to handle some cases like memory allocation. 

| Deserialization option name                         | Description                                                                   |
|-----------------------------------------------------|-------------------------------------------------------------------------------|
| **YAJP_DESERIALIZATION_OPTIONS_ALLOCATE**           | This option tells **YAJP** to allocate memory for field what holds JSON value |
| **YAJP_DESERIALIZATION_OPTIONS_ALLOCATE_ELEMENTS**  | This option tells **YAJP** to allocate memory for array elements              |

```c
#define YAJP_DESERIALIZATION_OPTIONS    (YAJP_DESERIALIZATION_OPTIONS_ALLOCATE | YAJP_DESERIALIZATION_OPTIONS_ALLOCATE_ELEMENTS)
```

If `YAJP_DESERIALIZATION_OPTIONS` is `YAJP_DESERIALIZATION_OPTIONS_ALLOCATE` **YAJP** expects field what will holds 
JSON value will be pointer. If `YAJP_DESERIALIZATION_OPTIONS` is `YAJP_DESERIALIZATION_OPTIONS_ALLOCATE` and field in 
not a pointer, compilation error will happened.  

#### <a id="sec-array_deserialization"></a> Array deserialization
Array deserialization is a complex process because nigher size of array, nor amount of array dimensions is unknown
until deserialization ends. Even if user knows array parameters it's impossible to make it more easier because JSON
supports arrays with non-constant amount of elements in each dimensions. To cover this case, **YAJP** expects that
deserializing array will be represented as a struct used to hold dynamic arrays, similar to something like this:
```c
typedef struct test_struct_arr test_struct_arr_t;
struct test_struct_arr {
    union {                     // union here can be replaced with struct if you want. Union is used to reduce amount of space
        int *elems;             // pointer to array used to store deserializing array values
        test_struct_arr_t *rows;// pointer to rows if array is multidimensional
    };
    bool final_dim;             // flag used to determine what structure holds array values. `false` if struct holds rows
    size_t count;               // amount of items in `elems` or `rows`
};
```
`final_dim` require additional attention. In case if **YAJP** deserializing matrix (2-dim array) next peace of code should works fine:
```c
typedef struct {
    test_struct_arr_t arr;
} deserializing_struct_t;
 
// field declaration
deserializing_struct_t test_struct;
 
// general deserialization routines
 
rows_count = test_struct.arr.count;                 // will show amount of rows in matrix
 
for (i = 0; i < rows_count; i++) {                  // traverse matrix
    assert(test_struct.arr.final_dim == false);
    columns_count = test_struct.arr.rows[i].count;  // amount of elements in row
    assert(test_struct.arr.rows[i].final_dim == true);
    for (int j = 0; j < columns_count; j++) {
        // do whatever we want with matrix element
        value = test_struct.arr.rows[i].elems[j];
    }
}
```
Here is an example of deserialization rule declaration for array field:
```c
typedef struct array_handle array_handle_t;
struct array_handle {
    union {
        void *elems;
        array_handle_t *rows;
    };
    bool final_dim;
    size_t count;
};

typedef struct {
    array_handle_t *arr;
} test_struct_t;

test_struct_t test_struct;
yajp_deserialization_rule_t actions[1];

// declare rules for test_struct_t.arr1
#define YAJP_DESERIALIZATION_STRUCT_FIELD_HOLDER_TYPE   test_struct_t
#define YAJP_DESERIALIZATION_STRUCT_FIELD_NAME          arr
#define YAJP_DESERIALIZATION_FIELD_TYPE                 (YAJP_DESERIALIZATION_TYPE_ARRAY_OF | YAJP_DESERIALIZATION_TYPE_NUMBER)
#define YAJP_DESERIALIZATION_OPTIONS                    (YAJP_DESERIALIZATION_OPTIONS_ALLOCATE_ELEMENTS | YAJP_DESERIALIZATION_OPTIONS_ALLOCATE)
#define YAJP_DESERIALIZATION_SETTER                     yajp_set_int

#define YAJP_DESERIALIZATION_ARRAY_ELEMENT_TYPE         int
#define YAJP_DESERIALIZATION_ARRAY_ELEMENTS             elems
#define YAJP_DESERIALIZATION_ARRAY_ROWS                 rows
#define YAJP_DESERIALIZATION_ARRAY_COUNTER              count
#define YAJP_DESERIALIZATION_ARRAY_FINAL_DIM            final_dim

#define YAJP_DESERIALIZATION_ACTION                     &actions[0]
#define YAJP_DESERIALIZATION_ACTION_INIT_RESULT         ret
#include <yajp/deserialization_action_initialization.h>
// ==========================================
if (ret) {
    perror("Failed to initialize action");
}
```

#### Deserialization example
See `tests/deserialization/deserialization_tests.c` for additional examples.
```c

```