#include "headers.h"

int main(int argc, char *argv[])
{
    while (clock() < atoi(argv[1]) * CLOCKS_PER_SEC);
    kill(getppid(),SIGCONT);
    return 0;
}