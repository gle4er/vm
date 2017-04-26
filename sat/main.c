#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "sc.h"

FILE *input = NULL;

void load_file(const char *filename)
{
    if ((input = fopen(filename, "r")) == NULL) {
        fprintf(stderr, "Cannot open file: no such file\n");
        exit(EXIT_FAILURE);
    }
    return;
}

void translating(const char *filename)
{
    FILE *output = NULL;
    if ((output = fopen(filename, "w")) == NULL) {
        fprintf(stderr, "Cannot setting output file\n");
        exit(EXIT_FAILURE);
    }
    for (int i = 0; feof(input); i++) {
        int inst_cnt = 0;
        fscanf(input, "%d", &inst_cnt);
        char command[10] = "\0";
        fscanf(input, "%s", command);
        if (strcmp(command, "READ")) {
            int dist = 0;
            fscanf(input, "%d", &dist);
            int value = 0;
            sc_commandEncode(10, dist, &value);
            fprintf(output, "%d", value);
        }
    }
}

int main(int argc, const char **argv)
{
    if (argc < 3) {
        fprintf(stderr, "Usage: %s input.sa output.o\n", argv[0]);
        exit(EXIT_FAILURE);
    }
    load_file(argv[1]);
    translating(argv[2]);
    return 0;
}
