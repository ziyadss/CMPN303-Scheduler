#include <stdio.h>
#include <stdlib.h>
#include <time.h>

int main(int argc, char *argv[])
{
    if (argc != 2)
        return fprintf(stderr, "Incorrect usage, to run use: ./bin/test_generator.out <count>\n");

    FILE *outFile = fopen("processes.txt", "w");
    if (outFile == NULL) // case couldn't read file
    {
        perror("Error opening output file");
        exit(-1);
    }

    int count = atoi(argv[1]);
    srand(time(NULL));

    fprintf(outFile, "#id arrival runtime priority\n");
    int arrivaltime = 1;
    for (int id = 1; id <= count; id++)
    {
        // [min-max] = rand() % (max_number + 1 - minimum_number) + minimum_number
        // rand() % x generates from 0 to 0-1
        arrivaltime += rand() % 11;
        int remainingtime = rand() % 30;
        int priority = rand() % 11;
        fprintf(outFile, "%d\t%d\t%d\t%d\n", id, arrivaltime, remainingtime, priority);
    }

    return fclose(outFile);
}
