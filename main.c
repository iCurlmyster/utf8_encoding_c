#include <stdio.h>
#include <stdlib.h>
#include <errno.h>
#include <string.h>
#include "utf8.h"

#define DESC_MAX 100

int main() {
    FILE *fp;
    char desc[DESC_MAX];
    memset(desc, 0, DESC_MAX);
    fp = fopen("./test.txt", "rb");
    if (fp == NULL) {
        perror("could not open file");
        exit(EXIT_FAILURE);
    }
    fread(desc, sizeof(char), DESC_MAX, fp);
    for (int i = 0; i < DESC_MAX; i++) {
        printf("\"%c\" ", desc[i]);
    }
    fclose(fp);
    bool success = valid_utf8(desc, DESC_MAX);
    if (success) {
        printf("string is valid utf8!\n");
    } else {
        printf("string is not valid utf8!\n");
    }
    return EXIT_SUCCESS;
}
