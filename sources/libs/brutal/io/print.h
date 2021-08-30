#pragma once

#include <brutal/base/count.h>
#include <brutal/base/map.h>
#include <brutal/base/std.h>
#include <brutal/io/write.h>
#include <brutal/text/str.h>

enum print_type
{
    PRINT_SIGNED,
    PRINT_UNSIGNED,
    PRINT_STRING,
    PRINT_POINTER,
    PRINT_CHAR,
};

struct print_value
{
    Str name;
    enum print_type type;

    union
    {
        long _signed;
        unsigned long long _unsigned;
        Str _string;
        void *_pointer;
        char _char;
    };
};

typedef struct
{
    size_t count;
    struct print_value *values;
} PrintArgs;

struct print_value print_val_signed(long);

struct print_value print_val_unsigned(unsigned long long);

struct print_value print_val_string(Str);

struct print_value print_val_cstring(char *);

struct print_value print_val_char(char);

struct print_value print_val_pointer(void *);

// clang-format off

#define PRINT_MATCH(VALUE)                      \
    _Generic((VALUE),                           \
        signed char: print_val_signed,          \
        signed short: print_val_signed,         \
        signed int: print_val_signed,           \
        signed long: print_val_signed,          \
	    signed long long: print_val_signed,     \
                                                \
        unsigned char: print_val_unsigned,      \
        unsigned short: print_val_unsigned,     \
        unsigned int: print_val_unsigned,       \
        unsigned long: print_val_unsigned,      \
	    unsigned long long: print_val_unsigned, \
        char*: print_val_cstring,               \
        char: print_val_char,                   \
        Str: print_val_string,                  \
        void*: print_val_pointer                \
    )(VALUE),

#define PRINT_NAMED(NAME, VALUE)                \
    ({                                          \
       auto print_value = PRINT_MATCH(VALUE);   \
       print_value.name = str$(NAME);       \
       print_value;                             \
    })

// clang-format on

#define PRINT_ARGS_(...) \
    (PrintArgs) { .count = 0, }

#define PRINT_ARGS_N(...) \
    (PrintArgs) { COUNT(__VA_ARGS__), (struct print_value[]){MAP(PRINT_MATCH, __VA_ARGS__)}, }

#define PRINT_ARGS(...) \
    PRINT_ARGS_##__VA_OPT__(N)(__VA_ARGS__)

IoWriteResult print_impl(IoWriter *writer, Str format, PrintArgs args);

#define print(writer, fmt, ...) \
    print_impl(writer, str$(fmt), PRINT_ARGS(__VA_ARGS__))
