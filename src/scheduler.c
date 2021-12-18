#include "headers.h"
#include "pcb.h"
#include "PriorityQueue.c"
#include "CircularQueue.c"

double WTA_sum = 0;
double WTA_SQ_sum = 0;
double waiting_sum = 0;
double burst_sum = 0;
size_t processCount;
int timestep = 0;

process *runningProcess = NULL;
int previousTimeStep;
key_t messageQueue;
Process_Table *processTable;
bool SRTNFlag = false;
FILE *outFile;

void addSums(double TA, int burstTime, int waitTime)
{
    double WTA = TA / burstTime;
    WTA_sum += WTA;
    WTA_SQ_sum += WTA * WTA;
    waiting_sum += waitTime;
    burst_sum += burstTime;
}

void SRTNHandler(int SIGNUM)
{
    SRTNFlag = true;
    signal(SIGNUM, SRTNHandler);
}

void createChildProcess(process *recievedProcess)
{
    int pid = fork();
    if (pid == 0)
    {
        // 2nd child: forked process
        char time[] = {[0 ... 10] = '\0'};
        sprintf(time, "%d", recievedProcess->remainingtime);
        execl("bin/process.out", "process.out", time, NULL);
    }
    else
    {
        // parent code: scheduler
        kill(pid, SIGTSTP);
        recievedProcess->processID = pid;
        PT_insert(processTable, recievedProcess);
    }
}

key_t getMessageQueue()
{
    key_t messageQueue = msgget(MSGKEY, 0666 | IPC_CREAT);

    if (messageQueue == -1)
    {
        perror("Error finding message queue from scheduler");
        exit(-1);
    }

    return messageQueue;
}

void HPF(PriorityQueue *processes)
{
    runningProcess = dequeuePQ(processes);

    if (runningProcess == NULL)
        return;

    int startTime = getClk();
    Pcb *block = PT_find(processTable, runningProcess);
    block->state = 1;

    fprintf(outFile, "At time %d process %d started arr %d total %d remain %d wait %d\n",
            startTime,
            block->uid,
            block->arrival_time,
            block->total_time,
            block->remaining_time,
            block->wait_time);

    kill(runningProcess->processID, SIGCONT);

    waitpid(runningProcess->processID, NULL, 0);

    block->burst_time = runningProcess->remainingtime;
    block->state = 2;
    block->remaining_time = 0;

    int finishTime = getClk();
    block->last_run_time = finishTime;

    double TA = finishTime - block->arrival_time;
    addSums(TA, block->burst_time, block->wait_time);

    fprintf(outFile, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %2f\n",
            finishTime,
            block->uid,
            block->arrival_time,
            block->burst_time,
            block->remaining_time,
            block->wait_time,
            (int)TA,
            TA / block->burst_time);

    free(runningProcess);
    runningProcess = NULL;
}

void fixWaitTimes(int curr, int *prev)
{
    for (int i = 0; i < (int)processCount; i++)
    {
        Pcb *block = processTable->table[i];
        if (block && block->state == 0)
        {
            int time = curr - max(block->last_run_time, *prev);
            block->wait_time += time;
            printf("Process %d wait time = %d\n", block->uid, block->wait_time);
        }
    }
    *prev = curr;
}

void HPFScheduler()
{
    PriorityQueue *queue = createPQ(processCount, priorityCompare);
    int prevTime = 0;
    while (true)
    {
        int currentTime = getClk();
        if (currentTime > prevTime)
            fixWaitTimes(currentTime, &prevTime);

        HPF(queue);

        process sp;
        while (msgrcv(messageQueue, &sp, sizeof(sp), 0, IPC_NOWAIT) != -1)
        {
            process *p = malloc(sizeof(*p));
            *p = sp;

            createChildProcess(p);
            enqueuePQ(queue, p);
            printf("Recievied process with id = %d, pid = %d at time = %d\n", p->id, p->processID, getClk());
        }
    }
}

process *RR(CircularQueue *processes)
{
    runningProcess = dequeueCQ(processes);

    if (runningProcess == NULL)
        return NULL;

    Pcb *block = PT_find(processTable, runningProcess);
    block->state = 1;

    char *action = (block->remaining_time == block->total_time) ? "At time %d process %d started arr %d total %d remain %d wait %d\n" : "At time %d process %d resumed arr %d total %d remain %d wait %d\n";

    int startTime = getClk();
    int runtime = min(runningProcess->remainingtime, QUANTUM_TIME);

    fprintf(outFile, action, startTime,
            block->uid,
            block->arrival_time,
            block->total_time,
            block->remaining_time,
            block->wait_time);

    kill(runningProcess->processID, SIGCONT);
    sleep(runtime);
    kill(runningProcess->processID, SIGTSTP);

    runningProcess->remainingtime -= runtime;
    block->remaining_time = runningProcess->remainingtime;
    block->burst_time += runtime;

    int finishTime = getClk();
    block->last_run_time = finishTime;

    if (runningProcess->remainingtime <= 0)
    {
        block->state = 2;
        free(runningProcess);

        double TA = finishTime - block->arrival_time;
        addSums(TA, block->burst_time, block->wait_time);

        fprintf(outFile, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %2f\n",
                finishTime,
                block->uid,
                block->arrival_time,
                block->burst_time,
                block->remaining_time,
                block->wait_time,
                (int)TA,
                TA / block->burst_time);

        return NULL;
    }
    else
    {
        block->state = 0;

        fprintf(outFile, "At time %d process %d stopped arr %d total %d remain %d wait %d\n",
                finishTime,
                block->uid,
                block->arrival_time,
                block->total_time,
                block->remaining_time,
                block->wait_time);

        return runningProcess;
    }
}

void RRScheduler()
{

    CircularQueue *queue = createCQ();
    int prevTime = 0;
    while (true)
    {
        int currentTime = getClk();
        if (currentTime > prevTime)
            fixWaitTimes(currentTime, &prevTime);

        process *last_run = RR(queue);

        process sp;
        while (msgrcv(messageQueue, &sp, sizeof(sp), 0, IPC_NOWAIT) != -1)
        {
            process *p = malloc(sizeof(*p));
            *p = sp;

            createChildProcess(p);
            enqueueCQ(queue, p);
            printf("Recievied process with id = %d, pid = %d at time = %d\n", p->id, p->processID, getClk());
        }

        if (last_run != NULL)
            enqueueCQ(queue, last_run);
    }
}

void SRTN(PriorityQueue *processes)
{
    runningProcess = dequeuePQ(processes);
    if (runningProcess == NULL)
        return;

    printf("started process with id %d and time %d \n", runningProcess->id, getClk());
    kill(runningProcess->processID, SIGCONT);

    if (previousTimeStep != getClk())
    {
        previousTimeStep = getClk();
        // updateWaitTimes(previousTimeStep);
    }

    runningProcess->remainingtime = sleep(runningProcess->remainingtime);

    if (SRTNFlag)
    {
        process sp;
        if (msgrcv(messageQueue, &sp, sizeof(sp), 0, IPC_NOWAIT) != -1)
        {
            process *p = (process *)malloc(sizeof(process));
            *p = sp;
            createChildProcess(p);
            enqueuePQ(processes, p);
            printf("Recievied process with id = %d, pid = %d at time = %d\n", p->id, p->processID, getClk());
        }
        else
            printf("the buffer is empty!");
        SRTNFlag = false;
    }

    if (runningProcess->remainingtime > peekPQ(processes)->remainingtime)
    {

        printf("stopped process with id %d and time %d\n", runningProcess->id, getClk());
        kill(runningProcess->processID, SIGTSTP);
        enqueuePQ(processes, runningProcess);

        runningProcess = dequeuePQ(processes);
        kill(runningProcess->processID, SIGCONT);
    }
    else
        printf("%d here with time %d", runningProcess->id, runningProcess->remainingtime);

    if (runningProcess->remainingtime <= 0)
    {
        printf("finished process with id %d and time %d\n", runningProcess->id, getClk());
        free(runningProcess);
        runningProcess = NULL;
    }
}

void SRTNScheduler()
{

    PriorityQueue *queue = createPQ(processCount, timeCompare);

    while (true)
    {

        process sp;

        while (msgrcv(messageQueue, &sp, sizeof(sp), 0, IPC_NOWAIT) != -1)
        {

            process *p = malloc(sizeof(*p));
            *p = sp;

            createChildProcess(p);
            enqueuePQ(queue, p);
            printf("Recievied process with id = %d, pid = %d at time = %d\n", p->id, p->processID, getClk());
        }
        SRTN(queue);
    }
}

void outputCalculations()
{
    FILE *perf = fopen("scheduler.perf", "w");
    double WTA_avg = WTA_sum / processCount;
    int finish_time = getClk();

    fprintf(perf, "CPU utilization = %2f%%\n", 100 * burst_sum / finish_time);
    fprintf(perf, "Avg WTA = %2f\n", WTA_avg);
    fprintf(perf, "Avg Waiting = %2f\n", waiting_sum / processCount);
    fprintf(perf, "Std WTA = %2f\n", sqrt(WTA_SQ_sum / processCount - WTA_avg * WTA_avg));

    PT_Free(processTable);

    exit(0);
}

int main(int argc, char *argv[])
{
    if (argc != 3)
        return fprintf(stderr, "Incorrect usage, to run use: ./bin/scheduler.out <algorithm number> <maximum capacity>\n");

    signal(SIGINT, outputCalculations);
    outFile = fopen("scheduler.log", "w");

    fprintf(outFile, "#At time x process y state arr w total z remain y wait k\n");

    messageQueue = getMessageQueue();
    processCount = atol(argv[2]);
    processTable = PT_Create(processCount);

    initClk();

    int algorithm = atoi(argv[1]);
    if (algorithm == 3)
        signal(SIGCONT, SRTNHandler);
    if (algorithm == 1)
        HPFScheduler();
    else if (algorithm == 2)
        RRScheduler();
    else if (algorithm == 3)
        SRTNScheduler();
    else
        fprintf(stderr, "Invalid algorithm number");

    destroyClk(true);

    return 0;
}
