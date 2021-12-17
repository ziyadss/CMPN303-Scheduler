#include "headers.h"
#include "PriorityQueue.c"
#include "CircularQueue.c"
bool isBusy = false;

void handler();
void CreateProcessChild(process *recievedProcess)
{
    int pidProcess, stat_loc_Process;

    pidProcess = fork();
    if (pidProcess == 0)
    { //2nd child: forked process
        char arrremainingtime[] = {[0 ... 10] = '\0'};
        sprintf(arrremainingtime, "%d", recievedProcess->remainingtime);
        execl("bin/process.out", "process.out", arrremainingtime, NULL);
    }
    else
    { // parent code: scheduler
        recievedProcess->processID = pidProcess;
        kill(pidProcess, SIGTSTP);
    }
    return;
}

key_t getProcessQueue()
{
    key_t ProcessQueue;
    ProcessQueue = msgget(MSGPROCSCED, 0666 | IPC_CREAT); // or msgget(12613, IPC_CREAT | 0644)

    if (ProcessQueue == -1)
    {
        perror("Error finding message queue from scheduler");
        exit(-1);
    }
    return ProcessQueue;
}

process *receiveProcess(key_t ProcessQueue, process receivedProcess)
{
    int rec_val;
    process *receivedProcessActual = malloc(sizeof(process));
    rec_val = msgrcv(ProcessQueue, &receivedProcess, sizeof(receivedProcess), 0, !IPC_NOWAIT);
    receivedProcessActual->arrivaltime = receivedProcess.arrivaltime;
    receivedProcessActual->id = receivedProcess.id;
    receivedProcessActual->remainingtime = receivedProcess.remainingtime;
    receivedProcessActual->priority = receivedProcess.priority;
    //printf("sent size is %ld",sizeof(receivedProcess));

    if (rec_val == -1)
    {
        // also check if message is empty
        //printf("process not received\n");
        return NULL;
    }
    //printf("process received\n");
    //printf("ID %d  %d  %d  %d\n",receivedProcess.id,receivedProcess.arrivaltime,receivedProcess.remainingtime,receivedProcess.priority );
    return receivedProcessActual;
}
int getPriority(int algorithmNumber, process *receivedProcess)
{
    int chpf = 0, csrtn = 0, crr = 0;
    switch (algorithmNumber)
    {
    case 1:
        chpf = 1;
        break;
    case 2:
        csrtn = 1;
        break;
    case 3:
        crr = 1;
        break;
    }
    int priority = chpf * receivedProcess->priority + csrtn * receivedProcess->remainingtime + crr * receivedProcess->arrivaltime;
    return priority;
}
void HighestPriorityFirst(struct PriorityQueue *Processes)
{
    if (isBusy == false)
    {
        process *runningProcess = peekPQ(Processes);
        //process *runningProcess  =malloc(sizeof(process));
        dequeuePQ(Processes);
        if (runningProcess)
        {
            isBusy = true;
            kill(runningProcess->processID, SIGCONT);
            printf("Process %d started at time = %d\n", runningProcess->id, getClk());
        }
    }
    return;
}

int main(int argc, char *argv[])
{
    initClk();
    signal(SIGCHLD, handler);
    struct process processArr[5];
    int algorithmNumber = 1;

    struct PriorityQueue *queue = createPQ(atoi(argv[1])); // create p queue takes capacity??
    key_t ProcessQueue = getProcessQueue();
    int n = 0;
    process *runningProcess; // pointer that points at the running process
    while (true)
    {
        process *receivedProcess = malloc(sizeof(process));
        receivedProcess = receiveProcess(ProcessQueue, *receivedProcess);
        if (receivedProcess != NULL)
        {
            n++;
            CreateProcessChild(receivedProcess);
            enqueuePQ(queue, receivedProcess);
            //printf("recievied process with id = %d with pid = %d at time = %d\n", receivedProcess->id,receivedProcess->processID ,getClk());
        }

        // switch cases/ algortithms calls are here
        if (isBusy == false)
        {
            if (algorithmNumber == 1)
            {
                HighestPriorityFirst(queue);
            }
        }
    }

    // TODO implement the scheduler :)
    // upon termination release the clock resources

    //destroyClk(true);

    // printf(" algorithm number recieved is %d", algorithmNumber );
    // create empty priority queue
    // enqueue process in priority queue

    // struct CircularQueue *ReadyQueue= createQueue();
    // set priority and set priority queue data with recieved data

    //destroyClk(true);
}
void handler()
{
    int temppid, status;
    temppid = waitpid(-1, &status, WNOHANG);
    // printf("child died\n");
    if (WIFEXITED(status))
    {
        isBusy = false;
        // printf("process ended\n");
    }
    return;
}

// if (n==5) // priority queue is replacing data with new input !!
// {
//     while(peekPQ(queue))
//     {
//         process *p = peekPQ(queue);
//         dequeuePQ(queue);
//         printf("%d  %d  %d  %d\n", p->id, p->arrivaltime, p->remainingtime, p->priority);
//     }
// }