#include <stdio.h>
#include <string.h>
#include "json.h"

void testInput(char *input) {
    printf("Testing input: %s\n", input);
    struct ParseJsonValueResult output = parseJson(input, 0);
    if (output.value == NULL) {
        printf("\tNo parse found.\n");
    } else {
        printf("\tResult: ");
        printJson(output.value);
        printf("\n");
        if (strlen(input) != output.cursor) {    
            printf("\tFinal cursor does not match strlen %d != %ld\n", output.cursor, strlen(input));
        }
    }
}

int main() {
    testInput("[]");
    testInput("[ ]");
    testInput("123");
    testInput(" 123 ");
    testInput("[1,2,3]");
    testInput("[ 1, 2, 3 ]");
    testInput("[ 1 , 2 , 3 ]");
    testInput("\"Hello\"");
    testInput("\"\"");
    testInput("{ \"name\": \"Bob\" }");
    testInput("{ \"name\": \"Bob\", \"age\": 4 }");
    testInput("{}");
    testInput("[{\"foo\": \"bar\"}, {\"garbage\": \"dump\"}]");
    testInput("{\"foo\": [\"bar\", 3]}");
    testInput("true");
    testInput("false");
    testInput("null");
    testInput("[ true, false, null ]");
}