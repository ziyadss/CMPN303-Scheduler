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
        {
            int time = curr - max(block->last_run_time, *prev);
            block->wait_time += time;
            printf("Process %d wait time = %d\n", block->uid, block->wait_time);
        }
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

    fprintf(outFile, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n",
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
            if (msgrcv(messageQueue, &buffer, sizeof(buffer), 0, IPC_NOWAIT) == -1)
                break;

            process *p = malloc(sizeof(*p));
            *p = buffer;

            createChildProcess(p);
            enqueuePQ(queue, p);
            printf("Received process with id = %d, pid = %d at time = %d\n", p->id, p->processID, getClk());
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

        fprintf(outFile, "At time %d process %d finished arr %d total %d remain %d wait %d TA %d WTA %.2f\n",
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

        while (true)
        {
            if (msgrcv(messageQueue, &buffer, sizeof(buffer), 0, IPC_NOWAIT) == -1)
                break;

            process *p = malloc(sizeof(*p));
            *p = buffer;

            createChildProcess(p);
            enqueueCQ(queue, p);
            printf("Received process with id = %d, pid = %d at time = %d\n", p->id, p->processID, getClk());
        }

        if (last_run != NULL)
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
    block->state = 1;

    char *action = (block->remaining_time == block->total_time) ? "At time %d process %d started arr %d total %d remain %d wait %d\n" : "At time %d process %d resumed arr %d total %d remain %d wait %d\n";

    int startTime = getClk();

    fprintf(outFile, action, startTime,
            block->uid,
            block->arrival_time,
            block->total_time,
            block->remaining_time,
            block->wait_time);

    kill(runningProcess->processID, SIGCONT);

    while (true)
    {
        // printf("sleep\n");
        runningProcess->remainingtime = sleep(runningProcess->remainingtime);
        // printf("wake up\n");
        int finishTime = getClk();
        block->last_run_time = finishTime;

        while (SRTNFlag)
        {
            if (msgrcv(messageQueue, &buffer, sizeof(buffer), 0, IPC_NOWAIT) == -1)
                break;

            process *p = malloc(sizeof(*p));
            *p = buffer;

            createChildProcess(p);
            enqueuePQ(processes, p);
            printf("Received process with id = %d, pid = %d at time = %d\n", p->id, p->processID, getClk());

            if (p->remainingtime < runningProcess->remainingtime)
            {
                kill(runningProcess->processID, SIGTSTP);
                // finishTime = getClk();
                finishTime = block->remaining_time - runningProcess->remainingtime + startTime;

                printf("rprm %d, brm %d, ft %d, st %d\n", runningProcess->remainingtime, block->remaining_time, finishTime, startTime);

                // runningProcess->remainingtime = block->remaining_time - (finishTime - startTime);

                // printf("rprm %d\n", runningProcess->remainingtime);

                enqueuePQ(processes, runningProcess);

                block->state = 0;

                block->burst_time += block->remaining_time - runningProcess->remainingtime;
                block->remaining_time = runningProcess->remainingtime;

                printf("bbt %d, brt %d\n", block->burst_time, block->remaining_time);

                fprintf(outFile, "At time %d process %d stopped arr %d total %d remain %d wait %d\n",
                        finishTime,
                        block->uid,
                        block->arrival_time,
                        block->total_time,
                        block->remaining_time,
                        block->wait_time);
                return;
            }
        }

        SRTNFlag = false;

        block->burst_time += block->remaining_time - runningProcess->remainingtime;
        block->remaining_time = runningProcess->remainingtime;

        if (runningProcess->remainingtime <= 0)
        {

            block->state = 2;
            free(runningProcess);

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
            return;
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

        while (msgrcv(messageQueue, &buffer, sizeof(buffer), 0, IPC_NOWAIT) != -1)
        {
            process *p = malloc(sizeof(*p));
            *p = buffer;

            createChildProcess(p);
            enqueuePQ(queue, p);
            printf("Received process with id = %d, pid = %d at time = %d\n", p->id, p->processID, getClk());
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