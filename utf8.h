#ifndef UTF8_SIMPLE_HEADER
#define UTF8_SIMPLE_HEADER

#include <stdbool.h>
#include <string.h>
#include <wchar.h>

// 
typedef struct {
    char* bytes;
    size_t len;
} rune;

//
typedef struct {
    rune* runes;
    size_t len;
    size_t cap;
} rune_array;

//
rune_array read_utf8(const char *, size_t);

//
bool valid_utf8(const char *, size_t);


//
void print_runes_array(rune_array*);

//
void init_runes_array(rune_array*, size_t);

//
void insert_rune(rune_array*, rune);

//
void free_runes_array(rune_array*);

//
wchar_t rune_value(rune);

#endif
