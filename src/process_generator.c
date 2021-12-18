#include "headers.h"
#include "PriorityQueue.c"
#include "CircularQueue.c"

key_t messageQueue;

void clearResources()
{
    msgctl(messageQueue, IPC_RMID, NULL);
    destroyClk(true);
    exit(0);
}

int main(int argc, char *argv[])
{
    if (argc != 2)
        return fprintf(stderr, "Incorrect usage, to run use: ./bin/process_generator.out <algorithm number>\n");

    signal(SIGINT, clearResources);

    FILE *inFile = fopen("processes.txt", "r");
    if (inFile == NULL)
    {
        perror("Error opening processes file");
        exit(-1);
    }

    CircularQueue *processQueue = createCQ();

    char *line;
    size_t len, numberOfProcesses = 0;
    while (getline(&line, &len, inFile) != -1)
        if (line[0] != '#')
        {
            numberOfProcesses++;

            process *p = malloc(sizeof(*p));
            sscanf(line, processFormatString, &p->id, &p->arrivaltime, &p->remainingtime, &p->priority);

            printf(processFormatString, p->id, p->arrivaltime, p->remainingtime, p->priority);
            enqueueCQ(processQueue, p);
        }

    fclose(inFile);

    pid_t childClk = fork();
    if (childClk == 0) // clk process
        execl("bin/clk.out", "clk.out", NULL);

    pid_t Childschd = fork();
    if (Childschd == 0) // scheduler process
    {
        char arr[] = {[0 ... 10] = '\0'};
        sprintf(arr, "%ld", numberOfProcesses);
        return execl("bin/scheduler.out", "scheduler.out", argv[1], arr, NULL);
    }

    // process generator process
    initClk();
    messageQueue = msgget(MSGKEY, 0666 | IPC_CREAT);
    if (messageQueue == -1)
    {
        perror("Error creating message queue");
        exit(-1);
    }

    while (isEmptyCQ(processQueue) == false)
    {
        process *p_pointer = peekCQ(processQueue);

        process p = *p_pointer;

        if (getClk() >= p.arrivaltime)
        {
            dequeueCQ(processQueue);
            free(p_pointer);

            int sendVal = msgsnd(messageQueue, &p, sizeof(p), !IPC_NOWAIT);
            if (sendVal == -1)
            {
                perror("Error sending message to scheduler");
                exit(-1);
            }
            else
            {
                printf("Process %d sent to scheduler\n", p.id);
                kill(Childschd, SIGCONT);
            }
            //i need to send a signal to the scheduler to
            //alert SRTN That there is a new process
            //how do i do that
        }
    }

    wait(NULL);
    clearResources();
}
