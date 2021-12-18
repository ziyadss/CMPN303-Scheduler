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
#include <math.h>

#define QUANTUM_TIME 4

/*
    TODO:

    Do I have queues use buffers for peek & dequeue, returning -1 for failures?

    #include <stdbool.h> instead of the enum below?
 */

const char *processFormatString = "%d\t%d\t%d\t%d\n";

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
    pid_t processID;
} process;

#define SHKEY 300
#define MSGKEY 400

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

process *createProcess(int arrivalTime, int pr, int remtime, pid_t pid)
{
    process *p = malloc(sizeof(*p));
    p->arrivaltime = arrivalTime;
    p->priority = pr;
    p->remainingtime = remtime;
    p->id = pid;
    return p;
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

int min(int a, int b)
{
    return a < b ? a : b;
}

int max(int a, int b)
{
    return a > b ? a : b;
}