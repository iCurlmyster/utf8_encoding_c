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

utf8_byte_t byte_type(char c);
int handle_bytes(const char *str, int i, size_t len, utf8_byte_t cur_t);
bool check_char_bounds_and_valid_seq(char arr[4], utf8_byte_t t);

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

rune grabRune(const char *str, int pos, size_t bc) {
    char* arr = (char *)malloc(bc * sizeof(char));
    for (int i = 0; i < bc; i++) {
        arr[i] = str[pos + i];
    }
    rune r = {
        .bytes = arr,
        .len = bc
    };
    return r;
}

rune_array read_utf8(const char *str, size_t len) {
    utf8_byte_t cur_t;
    int bc = 0;
    rune_array runes;
    if (len <= 0) return runes;
    init_runes_array(&runes, len / 2);
    for (int i = 0; i < len; ) {
        cur_t = byte_type(str[i]);
        switch (cur_t) {
            case NEXT:
            case INVALID:
                return runes;
            default:
                bc = handle_bytes(str, i, len, cur_t);
                break;
        }
        if (bc == -1) {
            return runes;
        }
        insert_rune(&runes, grabRune(str, i, bc));
        i += bc;
    }
    return runes;
}

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

void init_runes_array(rune_array *a, size_t initialSize) {
  a->runes = (rune *)malloc(initialSize * sizeof(rune));
  a->len = 0;
  a->cap = initialSize;
}

void insert_rune(rune_array *a, rune element) {
  if (a->len == a->cap) {
    a->cap += a->cap * .7;
    a->runes = (rune *)realloc(a->runes, a->cap * sizeof(rune));
  }
  a->runes[a->len++] = element;
}

void free_runes_array(rune_array *a) {
  free(a->runes);
  a->runes = NULL;
  a->len = a->cap = 0;
}

void print_runes_array(rune_array *a) {
    for (int i = 0; i < a->len; i++) {
        printf("%C", rune_value(a->runes[i]));
    }
}

wchar_t rune_value(rune r) {
    int a = 0, b = 0, c = 0, d = 0;
    wchar_t val = 0;
    switch (r.len) {
        case 1:
            val = (wchar_t) r.bytes[0];
            break;
        case 2:
            a = r.bytes[0] & FIVE_FREE_BITS;
            b = r.bytes[1] & SIX_FREE_BITS;
            val = (wchar_t) ((a << 6) | b);
            break;
        case 3:
            a = r.bytes[0] & FOUR_FREE_BITS;
            b = r.bytes[1] & SIX_FREE_BITS;
            c = r.bytes[2] & SIX_FREE_BITS;
            val = (wchar_t) ((a << 12) | (b << 6) | c);
            break;
        case 4:
            a = r.bytes[0] & THREE_FREE_BITS;
            b = r.bytes[1] & SIX_FREE_BITS;
            c = r.bytes[2] & SIX_FREE_BITS;
            d = r.bytes[3] & SIX_FREE_BITS;
            val = (wchar_t) ((a << 18) | (b << 12) | (c << 6) | d);
            break;
    }
    return val;
}
