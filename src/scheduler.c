#include "pcb.h"
#include "PriorityQueue.c"
#include "CircularQueue.c"

double WTA_sum = 0, WTA_SQ_sum = 0, waiting_sum = 0, burst_sum = 0;
size_t processCount;
int timestep = 0;

process *runningProcess = NULL;
key_t messageQueue;
Process_Table *processTable;
bool SRTNFlag = false;
FILE *outFile;

process buffer;

void addSums(double TA, int burstTime, int waitTime)
{
    double WTA = TA / burstTime;
    WTA_sum += WTA;
    WTA_SQ_sum += WTA * WTA;
    waiting_sum += waitTime;
    burst_sum += burstTime;
}

void logAction(Pcb *block, int startTime, char *action)
{

    fprintf(outFile, "At time %d process %d ", startTime, block->uid);
    fprintf(outFile, "%s ", action);
    fprintf(outFile, "arr %d total %d remain %d wait %d\n",
            block->arrival_time,
            block->total_time,
            block->remaining_time,
            block->wait_time);
}

void logFinish(Pcb *block, int finishTime)
{
    block->state = 2;
    block->remaining_time = 0;

    double TA = finishTime - block->arrival_time;
    addSums(TA, block->burst_time, block->wait_time);

    fprintf(outFile, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n",
            finishTime,
            block->uid,
            block->arrival_time,
            block->burst_time,
            block->remaining_time,
            block->wait_time,
            (int)TA,
            TA / block->burst_time);
}

void createChildProcess(process *receivedProcess)
{
    int pid = fork();
    if (pid == 0)
    {
        // 2nd child: forked process
        char time[] = {[0 ... 10] = '\0'};
        sprintf(time, "%d", receivedProcess->remainingtime);
        execl("bin/process.out", "process.out", time, NULL);
    }
    else
    {
        // parent code: scheduler
        kill(pid, SIGTSTP);
        receivedProcess->processID = pid;
        PT_insert(processTable, receivedProcess);
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

void fixWaitTimes(int curr, int *prev)
{
    for (int i = 0; i < (int)processCount; i++)
    {
        Pcb *block = processTable->table[i];
        if (block && block->state == 0)
            block->wait_time += curr - max(block->last_run_time, *prev);
    }
    *prev = curr;
}

void HPF(PriorityQueue *processes)
{
    runningProcess = dequeuePQ(processes);

    if (runningProcess == NULL)
        return;

    int startTime = getClk();
    Pcb *block = PT_find(processTable, runningProcess);

    block->state = 1;
    logAction(block, startTime, "started");

    kill(runningProcess->processID, SIGCONT);
    waitpid(runningProcess->processID, NULL, 0);

    block->burst_time = runningProcess->remainingtime;
    block->last_run_time = getClk();

    logFinish(block, block->last_run_time);

    free(runningProcess);
    runningProcess = NULL;
}

process *receiveProcess()
{
    if (msgrcv(messageQueue, &buffer, sizeof(buffer), 0, IPC_NOWAIT) == -1)
        return NULL;

    process *p = malloc(sizeof(*p));
    *p = buffer;

    createChildProcess(p);
    // printf("Received process with id = %d, pid = %d at time = %d\n", p->id, p->processID, getClk());

    return p;
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

        while (true)
        {
            process *p = receiveProcess();
            if (!p)
                break;
            enqueuePQ(queue, p);
        }
    }
}

process *RR(CircularQueue *processes)
{
    runningProcess = dequeueCQ(processes);

    if (runningProcess == NULL)
        return NULL;

    Pcb *block = PT_find(processTable, runningProcess);

    int startTime = getClk();
    int runtime = min(runningProcess->remainingtime, QUANTUM_TIME);

    block->state = 1;
    logAction(block, startTime, block->remaining_time < block->total_time ? "resumed" : "started");

    kill(runningProcess->processID, SIGCONT);
    sleep(runtime);
    kill(runningProcess->processID, SIGTSTP);

    runningProcess->remainingtime -= runtime;
    block->remaining_time = runningProcess->remainingtime;
    block->burst_time += runtime;

    block->last_run_time = getClk();

    if (runningProcess->remainingtime <= 0)
    {
        free(runningProcess);
        logFinish(block, block->last_run_time);
        return NULL;
    }
    else
    {
        block->state = 0;
        logAction(block, block->last_run_time, "stopped");
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

        while (true)
        {
            process *p = receiveProcess();
            if (!p)
                break;
            enqueueCQ(queue, p);
        }

        if (last_run)
            enqueueCQ(queue, last_run);
    }
}

void SRTNHandler()
{
    SRTNFlag = true;
    signal(SIGCONT, SRTNHandler);
}

void SRTN(PriorityQueue *processes)
{
    runningProcess = dequeuePQ(processes);

    if (runningProcess == NULL)
        return;

    Pcb *block = PT_find(processTable, runningProcess);

    int startTime = getClk();

    block->state = 1;
    logAction(block, startTime, block->remaining_time < block->total_time ? "resumed" : "started");

    kill(runningProcess->processID, SIGCONT);

    while (true)
    {

        runningProcess->remainingtime = sleep(runningProcess->remainingtime);
        block->last_run_time = getClk();

        while (SRTNFlag)
        {

            process *p = receiveProcess();
            if (!p)
                break;

            enqueuePQ(processes, p);

            if (p->remainingtime < runningProcess->remainingtime)
            {
                kill(runningProcess->processID, SIGTSTP);
                block->last_run_time = block->remaining_time - runningProcess->remainingtime + startTime;

                enqueuePQ(processes, runningProcess);

                block->state = 0;
                block->burst_time += block->remaining_time - runningProcess->remainingtime;
                block->remaining_time = runningProcess->remainingtime;

                return logAction(block, block->last_run_time, "stopped");
            }
        }

        SRTNFlag = false;

        block->burst_time += block->remaining_time - runningProcess->remainingtime;
        block->remaining_time = runningProcess->remainingtime;

        if (runningProcess->remainingtime <= 0)
        {
            logFinish(block, block->last_run_time);
            return free(runningProcess);
        }
    }
}

void SRTNScheduler()
{
    signal(SIGCONT, SRTNHandler);
    PriorityQueue *queue = createPQ(processCount, timeCompare);
    int prevTime = 0;
    while (true)
    {
        int currentTime = getClk();
        if (currentTime > prevTime)
            fixWaitTimes(currentTime, &prevTime);

        SRTN(queue);

        while (true)
        {
            process *p = receiveProcess();
            if (!p)
                break;
            enqueuePQ(queue, p);
        }
    }
}

void outputCalculations()
{
    FILE *perf = fopen("scheduler.perf", "w");
    double WTA_avg = WTA_sum / processCount;
    int finish_time = getClk();

    fprintf(perf, "CPU utilization = %.2f%%\n", 100 * burst_sum / finish_time);
    fprintf(perf, "Avg WTA = %.2f\n", WTA_avg);
    fprintf(perf, "Avg Waiting = %.2f\n", waiting_sum / processCount);
    fprintf(perf, "Std WTA = %.2f\n", sqrt(WTA_SQ_sum / processCount - WTA_avg * WTA_avg));

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