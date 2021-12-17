#include "headers.h"

int main(int argc, char *argv[])
{
    if (argc != 2)
        return fprintf(stderr, "Incorrect usage, to run use: ./bin/process.out <time>\n");

    int time = atoi(argv[1]);
    while (clock() < time * CLOCKS_PER_SEC);

    return 0;
}