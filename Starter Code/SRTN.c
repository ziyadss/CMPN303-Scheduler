#include "headers.h"
#include "../Data Structures/PriorityQueue.c"
#include "process_generator.c"
// to do:

bool insertProcess(PriorityQueue *q, process *b)
{
    return enqueuePQ(q, b);
}

bool checkRunning(PriorityQueue *q)
{
    /*look at the CURRENTLY running function in 
    comparison to the function at the beginning of the q*/
    process *current;
    return (timeCompare(current, peekPQ(q)));
}
