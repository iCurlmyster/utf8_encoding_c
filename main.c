#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include <locale.h>
#include <time.h>
#include "utf8.h"

rune_array grab_utf8(char *str, int size) {
    return read_utf8(str, size);
}

int main() {
    // set locale so unicode shows up in printf
    setlocale(LC_CTYPE, "en_US.UTF-8");

    FILE *fp;
    fp = fopen("./all_example.txt", "rb");
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
        printf("string is valid utf8!\n");
    } else {
        printf("string is not valid utf8!\n");
    }

    clock_t start, end;
    double cpu_time_used;
    start = clock();
    rune_array ra;
    for (int i = 0; i < 50; i++) {
        ra = grab_utf8(desc, file_size);
    }
    end = clock();
    cpu_time_used = ((double) (end - start)/ CLOCKS_PER_SEC);
    printf("time taken: %f\n", cpu_time_used);

    print_runes_array(&ra);

    free(desc);
    
    return EXIT_SUCCESS;
}
