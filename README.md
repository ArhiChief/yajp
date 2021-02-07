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

### Deserialization
Structures and functions used to declare deserialization rules are defined in `yajp/deserialization.h`. The way is to 
declare rules describing deserialization of each field in structure, called deserialization actions (`yajp_deserialization_action_t`).

After all actions are initialized they can be used as parameters for initializing deserialization context. Context is 
used as a container for deserialization actions. 

General function to create deserialization action is defined like this:
```c
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
```

Function is too generic and it's recommended to use macro wrappers how it's described in next sections.

#### Numbers and booleans deserialization
Deserialization actions for numbers and booleans can be initialized by calling `YAJP_PRIMITIVE_FIELD_DESERIALIZATION_ACTION_INIT` or
`YAJP_PRIMITIVE_FIELD_OVERWRITE_DESERIALIZATION_ACTION_INIT` function wrappers:

```c
#define YAJP_PRIMITIVE_FIELD_DESERIALIZATION_ACTION_INIT(structure, field, setter, action)
```
- `structure` - Type of structure where deserializing field is defined;
- `field` - Name of deserializing field;
- `setter` - pointer to function of type `yajp_value_setter_t` used to set convert string value in json to field type;
- `action` - pointer to `yajp_deserialization_action_t` to store action.

```c
#define YAJP_PRIMITIVE_FIELD_OVERWRITE_DESERIALIZATION_ACTION_INIT(json_field, structure, field, setter, action)
```
- `json_field` - Name of field in JSON stream.
- `structure` - Type of structure where deserializing field is defined;
- `field` - Name of deserializing field;
- `setter` - pointer to function of type `yajp_value_setter_t` used to set convert string value in json to field type;
- `action` - pointer to `yajp_deserialization_action_t` to store action.

Example of usage:
```c
typedef struct {
    int integer_field;
    int integer_field2;
} test_struct_t;

int result;

result = YAJP_PRIMITIVE_FIELD_DESERIALIZATION_ACTION_INIT(test_struct_t, integer_field, yajp_set_int, action_ptr);
// in case if JSON field name is called different
result = YAJP_PRIMITIVE_FIELD_OVERWRITE_DESERIALIZATION_ACTION_INIT("name", test_struct_t, integer_field, yajp_set_int, action2_ptr);
```

#### String deserialization
Deserialization of string is same as deserialization of other values except two points. **YAJP** expect that string in 
structure is null-terminated C string. Deserializing field can be defined as pointer to string or as array.
`YAJP_STRING_FIELD_DESERIALIZATION_ACTION_INIT` and `YAJP_STRING_FIELD_OVERWRITE_DESERIALIZATION_ACTION_INIT` are used 
to initialize deserialization actions for string fields.

```c
#define YAJP_STRING_FIELD_DESERIALIZATION_ACTION_INIT(structure, field, setter, allocate, action)
```
- `structure` - Type of structure where deserializing field is defined;
- `field` - Name of deserializing field;
- `setter` - pointer to function of type `yajp_value_setter_t` used to set convert string value in json to field type;
- `allocate` - pass `true` if memory for string should be allocated on heap and `false` if field is array;  
- `action` - pointer to `yajp_deserialization_action_t` to store action.

```c
#define YAJP_STRING_FIELD_OVERWRITE_DESERIALIZATION_ACTION_INIT(json_field, structure, field, setter, allocate, action)
```
- `json_field` - Name of field in JSON stream.
- `structure` - Type of structure where deserializing field is defined;
- `field` - Name of deserializing field;
- `setter` - pointer to function of type `yajp_value_setter_t` used to set convert string value in json to field type;
- `allocate` - pass `true` if memory for string should be allocated on heap and `false` if field is array;
- `action` - pointer to `yajp_deserialization_action_t` to store action.

Example of usage:
```c
typedef struct {
    char *string;
    char string2[30];
} test_struct_t;

int result;

result = YAJP_STRING_FIELD_DESERIALIZATION_ACTION_INIT(test_struct_t, string, yajp_set_int, true, action_ptr);

result = YAJP_STRING_FIELD_DESERIALIZATION_ACTION_INIT(test_struct_t, string2, yajp_set_int, false, action2_ptr);
```

#### Arrays deserialization
      TBD


#### Deserialization example. 
```c
#include <stdio.h>
#include <yajp/deserialization.h>
#include <yajp/deserialization_routine.h> // contains common setters

// dynamic array
typedef struct dyn_arr {
    union {
        void *elems;
        dyn_arr_t *rows;
    };
    
    size_t count;
    bool final_dim;
} dyn_arr_t;

// fixed array of ints
#define ARR_OF_INTS_CAP 10
typedef struct arr_of_ints {
    union {
        int elems[ARR_OF_INTS_CAP];
        arr_of_ints_t *rows;
    };

    size_t count;
    bool final_dim;
} arr_of_ints_t;

typedef struct test_struct {
    short           short_val;
    int             int_val;
    long int        long_int_val;
    long long int   long_long_int_val;
    float           float_val;
    double          double_val;
    long double     long_double_val;
    bool            bool_val;
    char[20]        string_val;         // string of fixed length. Ends with '\0'            
    char            *dyn_string_val;    // variable length string. Ends with '\0'

    dyn_arr_t       *dyn_arr_val1;      
    dyn_arr_t       arr_val1;    
    
    arr_of_ints_t   *dyn_arr_val2;
    arr_of_ints_t   arr_val2;
    
    dyn_arr_t       *strings_arr_val;

    dyn_arr_t       *matrix_val;        // 2-dimension array
    dyn_arr_t       *cube_val;          // 3-dimension array
} test_struct_t;

#define TEST_STRUCT_FIELDS_CNT  17
static yajp_deserialization_action_t    deserialization_actions[TEST_STRUCT_FIELDS_CNT];
static yajp_deserialization_ctx_t       deserialization_context;

// function is quite big, but it can be called once and result can be used multiple times.
static int init_deserialization_context() {
    int result;
    
    // for each field we should declare deserialization action, otherwise field will not be deserialized
    
    result = YAJP_PRIMITIVE_FIELD_DESERIALIZATION_ACTION_INIT(
            test_struct_t,                // deserializing structure type
            short_val,                    // field in deserializing structure
            yajp_set_short,               // value setter
            &deserialization_actions[0]   // pointer to holding structure
    );
                                                              
    if (0 != result) { return result; }

    result = YAJP_PRIMITIVE_FIELD_DESERIALIZATION_ACTION_INIT(test_struct_t, int_val, yajp_set_int, &deserialization_actions[1]);
    if (0 != result) { return result; }

    result = YAJP_PRIMITIVE_FIELD_DESERIALIZATION_ACTION_INIT(test_struct_t, long_int_val, yajp_set_long_int, &deserialization_actions[2]);
    if (0 != result) { return result; }

    result = YAJP_PRIMITIVE_FIELD_DESERIALIZATION_ACTION_INIT(test_struct_t, long_int_val, yajp_set_long_int, &deserialization_actions[3]);
    if (0 != result) { return result; }

    result = YAJP_PRIMITIVE_FIELD_DESERIALIZATION_ACTION_INIT(test_struct_t, long_long_int_val, yajp_set_long_long_int, &deserialization_actions[4]);
    if (0 != result) { return result; }

    result = YAJP_PRIMITIVE_FIELD_DESERIALIZATION_ACTION_INIT(test_struct_t, float_val, yajp_set_float, &deserialization_actions[5]);
    if (0 != result) { return result; }

    result = YAJP_PRIMITIVE_FIELD_DESERIALIZATION_ACTION_INIT(test_struct_t, double_val, yajp_set_double, &deserialization_actions[6]);
    if (0 != result) { return result; }

    result = YAJP_PRIMITIVE_FIELD_DESERIALIZATION_ACTION_INIT(test_struct_t, long_double_val, yajp_set_long_double, &deserialization_actions[7]);
    if (0 != result) { return result; }

    result = YAJP_PRIMITIVE_FIELD_DESERIALIZATION_ACTION_INIT(test_struct_t, bool_val, yajp_set_bool_val, &deserialization_actions[8]);
    if (0 != result) { return result; }
    
    // string values is one of special case what require additional info to be provided
    result = YAJP_STRING_FIELD_DESERIALIZATION_ACTION_INIT(
            test_struct_t,              // deserializing structure type
            string_val,                 // field in deserializing structure
            yajp_set_string,            // value setter
            false,                      // describes whenever heap allocation for sting is needed. In this case, "string_val" is array and no allocation needed  
            &deserialization_actions[8] // pointer to holding structure
    );
    if (0 != result) { return result; }

    result = YAJP_STRING_FIELD_DESERIALIZATION_ACTION_INIT(test_struct_t, string_val, dyn_string_val, true, &deserialization_actions[9]
    );
    if (0 != result) { return result; }
    
    // arrays are also a special case. See section Array Deserialization for details
    result = YAJP_ARRAY_OF_PRIMITIVE_FIELD_DESERIALIZATION_ACTION_INIT(
            test_struct_t,                  // deserializing structure type
            dyn_arr_val1,                   // field in deserializing structure
            dyn_arr_t,                      // type of structure used to hold array
            count,                          // name of field in array holding structure, used to count elements
            final_dim,                      // name of field in array holding structure, used to show what there is no more rows
            rows,                           // name of field in array holding structure, used to hold rows in array    
            elems,                          // name of field in array holding structure, used to hold array items
            int,                            // type of array element
            true,                           // describes whenever heap allocation for array handling structure is needed
            true,                           // describes whenever heap allocation for array elements is needed
            yajp_set_long_int,              // value setter
            &deserialization_actions[10]    // pointer to holding structure
    );
    if (0 != result) { return result; }

    result = YAJP_ARRAY_OF_PRIMITIVE_FIELD_DESERIALIZATION_ACTION_INIT( test_struct_t, arr_val1, dyn_arr_t, count, final_dim, rows, elems, int, false, true, yajp_set_long_int, &deserialization_actions[11]);
    if (0 != result) { return result; }

    result = YAJP_ARRAY_OF_PRIMITIVE_FIELD_DESERIALIZATION_ACTION_INIT( test_struct_t, dyn_arr_val2, arr_of_ints_t, count, final_dim, rows, elems, int, true, false, yajp_set_long_int, &deserialization_actions[12]);
    if (0 != result) { return result; }

    result = YAJP_ARRAY_OF_PRIMITIVE_FIELD_DESERIALIZATION_ACTION_INIT( test_struct_t, arr_val2, arr_of_ints_t, count, final_dim, rows, elems, int, false, false, yajp_set_long_int, &deserialization_actions[13]);
    if (0 != result) { return result; }

    // yajp expects what arrays of strings are represented as array of pointers to strings  
    result = YAJP_ARRAY_OF_PRIMITIVE_FIELD_DESERIALIZATION_ACTION_INIT( test_struct_t, strings_arr_val, dyn_arr_t, count, final_dim, rows, elems, char*, true, true, yajp_set_string, &deserialization_actions[13]);
    if (0 != result) { return result; }
    
    // yajp don't differentiate arrays, matrix of cubes. Unified structure is used to cover all cases
    result = YAJP_ARRAY_OF_PRIMITIVE_FIELD_DESERIALIZATION_ACTION_INIT( test_struct_t, matrix_val, dyn_arr_t, count, final_dim, rows, elems, int, false, true, yajp_set_long_int, &deserialization_actions[11]);
    if (0 != result) { return result; }

    result = YAJP_ARRAY_OF_PRIMITIVE_FIELD_DESERIALIZATION_ACTION_INIT( test_struct_t, cube_val, dyn_arr_t, count, final_dim, rows, elems, int, false, true, yajp_set_long_int, &deserialization_actions[11]);
    if (0 != result) { return result; }
    
    result = yajp_deserialization_ctx_init(deserialization_actions, TEST_STRUCT_FIELDS_CNT, &deserialization_context);
    return result;
}

int main(int argc, char **argv) {
    test_struct_t test_struct;
    yajp_deserialization_result result;
    char *result_str;
    
    if (0 != init_deserialization_context()) {
        perror("Deserialization context initialization failed");
        return -1;
    }

    result = yajp_deserialize_json_string(js, sizeof(js), &deserialization_context, &test_struct, NULL);

    result_str = yajp_deserialization_result_status_to_str(&result);
    
    printf("Deserialization finished with: %s\n", result_str);
    free(result_str);
    
    return (result.status == YAJP_DESERIALIZATION_RESULT_STATUS_OK) 
            ? 0 
            : -1;
}
```