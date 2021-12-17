#include "headers.h"

#include "PriorityQueue.c"
#include "CircularQueue.c"

void handler();

bool isBusy = false;
process *runningProcess = NULL;
key_t messageQueue;

void createChildProcess(process *recievedProcess)
{
    int pid = fork();
    if (pid == 0)
    {
        //2nd child: forked process
        char time[] = {[0 ... 10] = '\0'};
        sprintf(time, "%d", recievedProcess->remainingtime);
        execl("bin/process.out", "process.out", time, NULL);
    }
    else
    {
        // parent code: scheduler
        recievedProcess->processID = pid;
        kill(pid, SIGTSTP);
    }
    return;
}

key_t getMessageQueue()
{
    key_t messageQueue = msgget(MSGPROCSCED, 0666 | IPC_CREAT); // or msgget(12613, IPC_CREAT | 0644)

    if (messageQueue == -1)
    {
        perror("Error finding message queue from scheduler");
        exit(-1);
    }

    return messageQueue;
}

process *receiveProcess()
{
    process pp; // = malloc(sizeof(process));
    // int recVal = msgrcv(messageQueue, p, sizeof(*p), 0, IPC_NOWAIT);
    int recVal = msgrcv(messageQueue, &pp, sizeof(pp), 0, IPC_NOWAIT);

    if (recVal == -1)
    {
        // free(p);
        return NULL;
    }

    process *p = malloc(sizeof(*p));
    // p->arrivaltime = pp.arrivaltime;
    // p->remainingtime = pp.remainingtime;
    // p->priority = pp.priority;
    // p->processID = pp.processID;
    // p->id = pp.id;

    printf("Process received\n");
    // printf(processFormatString, p->id, p->arrivaltime, p->remainingtime, p->priority);
    return p;
}

void HighestPriorityFirst(struct PriorityQueue *Processes)
{

    runningProcess = dequeuePQ(Processes);

    if (runningProcess == NULL)
        return;

    isBusy = true;
    kill(runningProcess->processID, SIGCONT);

    printf("Process %d started at time = %d\n", runningProcess->id, getClk());

    int status;
    pid_t pid = wait(&status);
    printf("pp1 %d\tpp2 %d\n", (int)pid, runningProcess->processID);

    // while (runningProcess->processID != wait(&status));

    isBusy = false;

    printf("Process %d finished at time = %d\n", runningProcess->id, getClk());
}

void HPFScheduler(size_t capacity)
{

    PriorityQueue *queue = createPQ(capacity);

    while (true)
    {
        process *p = receiveProcess();
        while (p != NULL)
        {
            createChildProcess(p);
            enqueuePQ(queue, p);
            printf("Recievied process with id = %d, pid = %d at time = %d\n", p->id, p->processID, getClk());
            p = receiveProcess();
        }

        HighestPriorityFirst(queue);
    }
}

int main(int argc, char *argv[])
{
    if (argc != 2)
        // return fprintf(stderr, "Incorrect usage, to run use: ./bin/scheduler.out <algorithm number>\n");
        return fprintf(stderr, "Incorrect usage, to run use: ./bin/scheduler.out <maximum capacity>\n");

    initClk();
    // signal(SIGCHLD, childHandler);

    messageQueue = getMessageQueue();

    printf("TIME: %d\n", getClk());

    HPFScheduler(atol(argv[1]));

    destroyClk(true);
}

// void childHandler()
// {
//     int status;
//     waitpid(runningProcess->processID, &status, !WNOHANG);

//     if (WIFEXITED(status))
//         isBusy = false;

//     signal(SIGCHLD, childHandler);
// }

// if (n==5) // priority queue is replacing data with new input !!
// {
//     while(peekPQ(queue))
//     {
//         process *p = peekPQ(queue);
//         dequeuePQ(queue);
//         printf("%d  %d  %d  %d\n", p->id, p->arrivaltime, p->remainingtime, p->priority);
//     }
// }