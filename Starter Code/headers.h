#pragma once

#include <stdio.h> //if you don't use scanf/printf change this include
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/file.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <time.h>
#include <string.h>

/*
    TODO:

    Have a predicate function passed to createPQ to use for that instance for priorities? (Ask Abdo?)

    Do I have queues use buffers for peek & dequeue, returning -1 for failures?

    #include <stdbool.h> instead of the enum below?
 */

// Boolean datatype
typedef enum
{
    false,
    true
} bool;

typedef struct process
{
    int arrivaltime;
    int priority;
    int remainingtime;
    int id;
    int processID;
} process;

#define SHKEY 300
#define MSGPROCSCED 400

///==============================
// don't mess with this variable//
int *shmaddr; //
//===============================

int getClk()
{
    return *shmaddr;
}

/*
 * All process call this function at the beginning to establish communication between them and the clock module.
 * Again, remember that the clock is only emulation!
 */
void initClk()
{
    int shmid = shmget(SHKEY, 4, 0444);
    // printf("hello\n");
    while ((int)shmid == -1)
    {
        // Make sure that the clock exists
        printf("Wait! The clock not initialized yet!\n");
        sleep(1);
        shmid = shmget(SHKEY, 4, 0444);
    }
    shmaddr = (int *)shmat(shmid, NULL, 0);
}

process *createProcess(int arrivalTime, int pr, int remtime, int pid)
{
    process *p = malloc(sizeof(*p));
    p->arrivaltime = arrivalTime;
    p->priority = pr;
    p->remainingtime = remtime;
    p->id = pid;
    return p;
}

bool timeCompare(process *a, process *b)
{
    return (a->remainingtime > b->remainingtime);
}

/*
 * All process call this function at the end to release the communication
 * resources between them and the clock module.
 * Again, Remember that the clock is only emulation!
 * Input: terminateAll: a flag to indicate whether that this is the end of simulation.
 *                      It terminates the whole system and releases resources.
 */

void destroyClk(bool terminateAll)
{
    shmdt(shmaddr);
    if (terminateAll)
        killpg(getpgrp(), SIGINT);
}

char *convertIntegerToChar(int N)
{
    int m = N;
    int digit = 0;
    while (m)
    {
        digit++;
        m /= 10;
    }
    char *arr;
    char arr1[digit];
    arr = (char *)malloc(digit);
    int index = 0;
    while (N)
    {
        arr1[++index] = N % 10 + '0';
        N /= 10;
    }
    int i;
    for (i = 0; i < index; i++)
    {
        arr[i] = arr1[index - i];
    }
    arr[i] = '\0';
    return (char *)arr;
}