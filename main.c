#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <locale.h>
#include "utf8.h"

int main() {
    // set locale so unicode shows up in printf
    setlocale(LC_CTYPE, "en_US.UTF-8");

    FILE *fp;
    fp = fopen("./examples/all_example.txt", "r");
    if (fp == NULL) {
        perror("could not open file");
        exit(EXIT_FAILURE);
    }

    fseek(fp, 0L, SEEK_END);
    long file_size = ftell(fp);
    rewind(fp);
    char *desc = malloc(file_size * sizeof(char));
    
    fread(desc, sizeof(char), file_size, fp);
    fclose(fp);

    // Validate UTF8
    bool success = valid_utf8(desc, file_size);
    if (success) {
        printf("string is valid utf8!\n\n");
    } else {
        printf("string is not valid utf8!\n\n");
    }

    // read utf8 characters
    utf8_array ra = read_utf8(desc, file_size);
    print_utf8_array(&ra);

    free_utf8_array(&ra);
    free(desc);
    
    return EXIT_SUCCESS;
}
