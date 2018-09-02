#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <locale.h>
#include "utf8.h"

#define DESC_MAX 100

int main() {
    // set locale so unicode shows up in printf
    setlocale(LC_CTYPE, "en_US.UTF-8");

    FILE *fp;
    char desc[DESC_MAX];
    memset(desc, 0, DESC_MAX);
    fp = fopen("./test.txt", "rb");
    if (fp == NULL) {
        perror("could not open file");
        exit(EXIT_FAILURE);
    }
    fread(desc, sizeof(char), DESC_MAX, fp);
    fclose(fp);

    // Validate UTF8
    bool success = valid_utf8(desc, DESC_MAX);
    if (success) {
        printf("string is valid utf8!\n");
    } else {
        printf("string is not valid utf8!\n");
    }

    // Grab UTF8 string and print it
    rune_array ra = read_utf8(desc, DESC_MAX);
    print_runes_array(&ra);
    
    return EXIT_SUCCESS;
}
