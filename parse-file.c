#include <stdio.h>
#include <stdlib.h>
#include "json.h"

int main(int argc, char *argv[]) {
    /* Stolen from https://stackoverflow.com/questions/14002954/c-programming-how-to-read-the-whole-file-contents-into-a-buffer */
    if (argc < 2) {
        printf("Please provide a file name.\n");
        return 1;
    }
    char *filename = argv[1];
    printf("Opening %s\n", filename);
    FILE *f = fopen(filename, "r");
    fseek(f, 0, SEEK_END);
    long fsize = ftell(f);
    fseek(f, 0, SEEK_SET);  /* same as rewind(f); */

    char *string = malloc(fsize + 1);
    fread(string, 1, fsize, f);
    fclose(f);

    string[fsize] = 0;
    struct ParseJsonValueResult result = parseJson(string, 0);
    if (result.value) {
        printJson(result.value);
        printf("\n");
    } else {
        printf("Invalid json\n");
    }
}