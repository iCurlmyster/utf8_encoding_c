#ifndef UTF8_SIMPLE_HEADER
#define UTF8_SIMPLE_HEADER

#include <stdbool.h>
#include <stddef.h>

// utf8_array is a simple structure that holds an array of wchar_t
// bytes is an array of wchar_t
// len is the current size of bytes that is used
// cap is the current potential size of bytes that can be used
typedef struct {
    wchar_t* bytes;
    size_t len;
    size_t cap;
} utf8_array;

// read_utf8 takes a regular char array and size and returns a utf8_array object
utf8_array read_utf8(const char *, size_t);

// valid_utf8 validates the given string
bool valid_utf8(const char *, size_t);

// print_utf8_array prints the values to stdout
void print_utf8_array(utf8_array*);

// init_utf8_array initializes an utf8_array object to capacity of the given size
void init_utf8_array(utf8_array*, size_t);

// insert_utf8_array inserts a new wchar_t onto the given array
void insert_utf8_byte(utf8_array*, wchar_t);

// free_utf8_array frees the memory of the internal objects
void free_utf8_array(utf8_array*);

#endif
