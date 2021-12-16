 #include "headers.h"

int main(int argc, char * argv[])
{
    
    while(clock() < atoi(argv[1]) * CLOCKS_PER_SEC)
    {
        clock_t cycles = clock();
        double time = (double)cycles;
        // if((int)time%1000000>999990)
            // printf("clocks passed %lf out of %ld\n",time, atoi(argv[1]) * CLOCKS_PER_SEC);
    }
    // printf("child end of code\n");
    return 0;
}