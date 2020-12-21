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
| YAJP_TRACK_STREAM      | BOOL   | ON             | Track parsing stream. In case of error, line and collumn number with error will be returned. Not implemented for now.                                                                    |
| YAJP_BUFFER_SIZE       | STRING | 32             | Size in bytes of buffers used to work with JSON. If value can't be fitted into buffer it will be extended enough to handle value and size will be multiplicable by **YAJP_BUFFER_SIZE**. | 

## Usage

**YAJP** can be added as dependency to your CMake project as a subdirectory or as package obtained by `find-package`:
 - If **YAJP** is added to your project as a subproject:
```cmake
include(<path/to/yajp/sources>)
```
 - If yajp is installed to your sysroot than `find-package` cmake command can be used:
```cmake
find_package(yajp REQUIRED)
```

After that, in can be linked with your targets like this:
```cmake
target_link_libraries(target_name PRIVATE yajp::yajp_lib)
```

### Deserialization example
For now, support is very limited and can handle only deserialization of plain object data structures. Here how it can be
done:

```c
#include <yajp/deserialization.h>
#include <yajp/deserialization_routine.h> // yajp_parse_int() is defined here

#include <string.h>

typedef struct {
    int int_field1;
    int other_field;
} test_struct_t;

static const yajp_deserialization_action_t actions[2];
static const yajp_deserialization_ctx_t ctx; 

static int init_deserialization_declarations() {
    int ret;
    
    // declare rules actions what will describe how deserialization should go
    ret = YAJP_PRIMITIVE_FIELD_DESERIALIZATION_ACTION_INIT(
            test_struct_t,  // type of structure where deserializing field is stored
            int_field1,     // name of field in deserializing structure
            yajp_parse_int, // pointer to function used to deserialize JSON value into structure field. 
            &actions[0]     // pointer to initializing deserialization action
            );

    if (0 != ret) {
        return ret;
    }
    
    // in case if JSON field name is not equal to deserializing field name, this macro can be used 
    ret = YAJP_PRIMITIVE_FIELD_OVERWRITE_DESERIALIZATION_ACTION_INIT(
            "int_field2",   // name of field in JSON stream
            test_struct_t,  // type of structure where deserializing field is stored
            other_field,    // name of field in deserializing structure
            yajp_parse_int, // pointer to function used to deserialize JSON value into structure field. 
            &actions[0]     // pointer to initializing deserialization action
            );

    if (0 != ret) {
        return ret;
    }

    ret = yajp_deserialization_ctx_init(actions, (sizeof(actions) / sizeof(actions[0])), &ctx);
    
    return ret;
}

int deserialize() {
    const char *json = "{\"int_field1\":12345, \"other_field\":332}";
    size_t json_size = strlen(json) * sizeof(*json);
    yajp_deserialization_result_t ret;
    test_struct_t result;

    ret = init_deserialization_declarations();

    if (0 != ret) {
        ret = yajp_deserialize_json_string(json, json_size, &ctx, &result, NULL);
    }
    
    return ret;
}
```

### API Documentation
**YAJP** provides next headers with definations of functions, structures, macros, etc:
 - `yajp/deserialization.h`. Provides interfaces for deserialization routines;
 - `yajp/deserialization_routines.h`. Provides functions what can be used to deserialize C structure fields from JSON values.

`yajp/deserialization.h` provides next definitions:

// TODO!!


