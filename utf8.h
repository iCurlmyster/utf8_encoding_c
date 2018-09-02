#ifndef UTF8_SIMPLE_HEADER
#define UTF8_SIMPLE_HEADER

#include <stdbool.h>
#include <stddef.h>

// utf8_array is a simple structure that holds an array of 
typedef struct {
    wchar_t* bytes;
    size_t len;
    size_t cap;
} utf8_array;

//
utf8_array read_utf8(const char *, size_t);

//
bool valid_utf8(const char *, size_t);


//
void print_utf8_array(utf8_array*);

//
void init_utf8_array(utf8_array*, size_t);

//
void insert_utf8_byte(utf8_array*, wchar_t);

//
void free_utf8_array(utf8_array*);

wchar_t utf8_value(const char *, int, size_t);

#endif
