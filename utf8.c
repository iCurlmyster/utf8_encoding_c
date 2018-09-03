#include "utf8.h"
#include <stdio.h>
#include <stdlib.h>

#define SINGLE_MAX 127
#define DOUBLE_MAX 2047
#define TRIPLE_MAX 65535
#define QUAD_MAX 1114111

#define SIX_FREE_BITS 63
#define FIVE_FREE_BITS 31
#define FOUR_FREE_BITS 15
#define THREE_FREE_BITS 7

#define ONE_BYTE_FLAG(n) !((n >> 7) & 1)
#define NEXT_BYTE_FLAG(n) (((n >> 6) & 3) == 2)
#define TWO_BYTE_FLAG(n) (((n >> 5) & 7) == 6)
#define THREE_BYTE_FLAG(n) (((n >> 4) & 15) == 14)
#define FOUR_BYTE_FLAG(n) (((n >> 3) & 31) == 30)

typedef enum {
    ONE,
    TWO,
    THREE,
    FOUR,
    NEXT,
    INVALID
} utf8_byte_t;

utf8_byte_t byte_type(char);
int handle_bytes(const char *, int, size_t, utf8_byte_t);
bool check_char_bounds_and_valid_seq(char[4], utf8_byte_t);
wchar_t utf8_value(const char *, int, size_t);

//  public functions

bool valid_utf8(const char *str, size_t len) {
    utf8_byte_t cur_t;
    int bc = 0;
    for (int i = 0; i < len; ) {
        cur_t = byte_type(str[i]);
        switch (cur_t) {
            case NEXT:
            case INVALID:
                return false;
            default:
                bc = handle_bytes(str, i, len, cur_t);
                break;
        }
        if (bc == -1) {
            return false;
        }
        i += bc;
    }
    return true;
}

utf8_array read_utf8(const char *str, size_t len) {
    utf8_byte_t cur_t;
    int bc = 0;
    utf8_array bytes;
    if (len <= 0) return bytes;
    init_utf8_array(&bytes, len / 2);
    for (int i = 0; i < len; ) {
        cur_t = byte_type(str[i]);
        switch (cur_t) {
            case NEXT:
            case INVALID:
                return bytes;
            default:
                bc = handle_bytes(str, i, len, cur_t);
                break;
        }
        if (bc == -1) {
            return bytes;
        }
        insert_utf8_byte(&bytes, utf8_value(str, i, bc));
        i += bc;
    }
    return bytes;
}


void init_utf8_array(utf8_array *a, size_t initialSize) {
  a->bytes = (wchar_t *)malloc(initialSize * sizeof(wchar_t));
  a->len = 0;
  a->cap = initialSize;
}

void insert_utf8_byte(utf8_array *a, wchar_t element) {
  if (a->len == a->cap) {
    a->cap += a->cap * .7;
    a->bytes = (wchar_t *)realloc(a->bytes, a->cap * sizeof(wchar_t));
  }
  a->bytes[a->len++] = element;
}

void free_utf8_array(utf8_array *a) {
  free(a->bytes);
  a->bytes = NULL;
  a->len = a->cap = 0;
}

void print_utf8_array(utf8_array *a) {
    for (int i = 0; i < a->len; i++) {
        printf("%C", a->bytes[i]);
    }
}

// private functions

int handle_bytes(const char *str, int i, size_t len, utf8_byte_t cur_t) {
    char data[4];
    switch (cur_t) {
        case ONE:
            return 1;
        case TWO:
            if ((i+1) >= len) return -1;
            data[0] = str[i];
            data[1] = str[i + 1];
            if (!check_char_bounds_and_valid_seq(data, cur_t)) return -1;
            return 2;
        case THREE:
            if ((i+2) >= len) return -1;
            data[0] = str[i];
            data[1] = str[i + 1];
            data[2] = str[i + 2];
            if (!check_char_bounds_and_valid_seq(data, cur_t)) return -1;
            return 3;
        case FOUR:
            if ((i+3) >= len) return -1;
            data[0] = str[i];
            data[1] = str[i + 1];
            data[2] = str[i + 2];
            data[3] = str[i + 3];
            if (!check_char_bounds_and_valid_seq(data, cur_t)) return -1;
            return 4;
        default:
            return -1;
    }
}

utf8_byte_t byte_type(char c) {
    if (ONE_BYTE_FLAG(c)) {
        return ONE;
    } else if (NEXT_BYTE_FLAG(c)) {
        return NEXT;
    } else if (TWO_BYTE_FLAG(c)) {
        return TWO;
    } else if (THREE_BYTE_FLAG(c)) {
        return THREE;
    } else if (FOUR_BYTE_FLAG(c)) {
        return FOUR;
    }
    return INVALID;
}

bool check_char_bounds_and_valid_seq(char arr[4], utf8_byte_t t) {
    switch (t) {
        case ONE:
            return arr[0] <= SINGLE_MAX;
        case TWO:
            if (!NEXT_BYTE_FLAG(arr[1])) return false;
            return true;
        case THREE:
            if (!NEXT_BYTE_FLAG(arr[1])) return false;
            if (!NEXT_BYTE_FLAG(arr[2])) return false;
            return true;
        case FOUR:
            if (!NEXT_BYTE_FLAG(arr[1])) return false;
            if (!NEXT_BYTE_FLAG(arr[2])) return false;
            if (!NEXT_BYTE_FLAG(arr[3])) return false;
            return true;
        default:
            return false;
    }
}

wchar_t utf8_value(const char *str, int pos, size_t bc) {
    int a = 0, b = 0, c = 0, d = 0;
    wchar_t val = 0;
    switch (bc) {
        case 1:
            val = (wchar_t) str[pos];
            break;
        case 2:
            a = str[pos] & FIVE_FREE_BITS;
            b = str[pos + 1] & SIX_FREE_BITS;
            val = (wchar_t) ((a << 6) | b);
            break;
        case 3:
            a = str[pos] & FOUR_FREE_BITS;
            b = str[pos + 1] & SIX_FREE_BITS;
            c = str[pos + 2] & SIX_FREE_BITS;
            val = (wchar_t) ((a << 12) | (b << 6) | c);
            break;
        case 4:
            a = str[pos] & THREE_FREE_BITS;
            b = str[pos + 1] & SIX_FREE_BITS;
            c = str[pos + 2] & SIX_FREE_BITS;
            d = str[pos + 3] & SIX_FREE_BITS;
            val = (wchar_t) ((a << 18) | (b << 12) | (c << 6) | d);
            break;
    }
    return val;
}
