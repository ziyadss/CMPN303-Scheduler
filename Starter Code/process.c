#include "headers.h"

/* Modify this file as needed*/
int remainingtime;

int main(int agrc, char *argv[])
{
    initClk();

    //TODO it needs to get the remaining time from somewhere
    //remainingtime = ??;
    int prevtime = 0;
    while (remainingtime > 0)
    {
        clock_t cycles = clock();
        int time = (double)cycles / CLOCKS_PER_SEC;
        if (time - prevtime == 1)
        {
            remainingtime--;
        }

        printf(" time elapsed %d\n", time);
        prevtime = time;
        // remainingtime = ??;
    }

    destroyClk(false);

    return 0;
}
