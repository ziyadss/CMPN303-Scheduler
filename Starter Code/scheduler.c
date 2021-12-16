#include "headers.h"
#include "../Data Structures/PriorityQueue.c"
#include "../Data Structures/CircularQueue.c"
bool isBusy = false;
// struct ProcessControlBlock
// {
//     int arrivaltime;
//     int excutiontime;
//     int remainingtime;
//     int waitingtime;
//     int id;
//     int status = 0; // 1 for running, 0 for waiting
// };
void handler(int signum);
void CreateProcessChild(process *recievedProcess)
{
    int pidProcess, stat_loc_Process;
    char *arrremainingtime = convertIntegerToChar(recievedProcess->remainingtime);
    pidProcess = fork();
    if (pidProcess == 0)
    { //2nd child: forked process
        char *argsProcess[] = {"./process.out", arrremainingtime, NULL};
        execv(argsProcess[0], argsProcess);
    }
    else
    { // parent code: scheduler
        recievedProcess->processID = pidProcess;
        // printf("child created with id %d\n",pidProcess);
        kill(pidProcess, SIGTSTP);
        // printf("child stopped");
    }
    return;
}

key_t getProcessQueue()
{
    key_t ProcessQueue;
    ProcessQueue = msgget(MSGPROCSCED, 0666 | IPC_CREAT); // or msgget(12613, IPC_CREAT | 0644)

    if (ProcessQueue == -1)
    {
        printf("msg queue not found\n");
        exit(-1);
    }
    return ProcessQueue;
}

bool receiveProcess(key_t ProcessQueue, process *receivedProcess)
{
    int rec_val;
    rec_val = msgrcv(ProcessQueue, &*receivedProcess, sizeof(*receivedProcess), 0, !IPC_NOWAIT);

    if (rec_val == -1)
    {
        // also check if message is empty
        //printf("process not received\n");
        return false;
    }
    //printf("process received\n");
    //printf("ID %d  %d  %d  %d\n",receivedProcess->id,receivedProcess->arrivaltime,receivedProcess->remainingtime,receivedProcess->priority );
    return true;
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
void HighestPriorityFirst(struct PriorityQueue *Processes, process *runningProcess)
{
    if (isBusy == false)
    {
        runningProcess = dequeuePQ(Processes);
        if (runningProcess)
        {
            isBusy = true;
            kill(runningProcess->processID, SIGCONT);
            // printf("process %d started with pid %d\n", runningProcess->id, runningProcess->processID);
        }
    }
    return;
}
int main(int argc, char *argv[])
{
    // printf("scheduler's id is %d\n", getpid());
    initClk();
    signal(SIGCHLD, handler);
    // process *runningProcess=malloc(sizeof(process));
    int rec_val;
    // int algorithmNumber = atoi(argv[1]);
    int algorithmNumber = 1;
    process *receivedProcess = malloc(sizeof(process));
    struct PriorityQueue *queue = createPQ(atoi(argv[1])); // create p queue takes capacity??
    key_t ProcessQueue = getProcessQueue();
    process *runningProcess = NULL; // pointer that points at the running process
    while (true)
    {

        bool received = receiveProcess(ProcessQueue, receivedProcess);
        //printf("recieved at clock =  %d\n",getClk());
        if (received == true)
        {
            CreateProcessChild(receivedProcess);
            receivedProcess->priority = getPriority(algorithmNumber, receivedProcess);
            enqueuePQ(queue, receivedProcess);
            // printf("%d  %d  %d  %d\n", receivedProcess->id, receivedProcess->arrivaltime, receivedProcess->remainingtime, receivedProcess->priority);
            // uncomment the next lines to check priority queue
            // if (receivedProcess->id==5) // priority queue is replacing data with new input !!
            // {
            //     while(peekPQ(queue))
            //     {
            //         process *p=malloc(sizeof(process));
            //         p=dequeuePQ(queue);
            //         printf("%d  %d  %d  %d\n", p->id, p->arrivaltime, p->remainingtime, p->priority);
            //     }
            // }
        }
        if (isBusy == false)
        {
            if (algorithmNumber == 1)
            {
                HighestPriorityFirst(queue, runningProcess);
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
void handler(int signum)
{
    int temppid, status;
    temppid = waitpid(-1,&status,WNOHANG);
    // printf("child died\n");
    if (WIFEXITED(status))
    {
        isBusy = false;
        // printf("process ended\n");
    }
    return;
}