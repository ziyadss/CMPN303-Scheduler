#include "headers.h"

void CreateProcessChild(int runningTime){
    int pidProcess, stat_loc_Process;
    char * arrRunningTime=convertIntegerToChar(runningTime);
    pidProcess = fork();
        if (pidProcess == 0)
        { //2nd child: scheduler creation
            char *argsProcess[] = {"./process.out",arrRunningTime,NULL};
            execv(argsProcess[0], argsProcess);
        }
        else
        return;
}


key_t getProcessQueue()
{
    key_t ProcessQueue;
    ProcessQueue = msgget( MSGPROCSCED, 0666 | IPC_CREAT); // or msgget(12613, IPC_CREAT | 0644)

    if (ProcessQueue == -1)
    {
        printf("msg queue not found");
        exit(-1);
    }
    return ProcessQueue;
}

bool receiveProcess(key_t ProcessQueue, process *receivedProcess)
{
    int rec_val;
    rec_val = msgrcv(ProcessQueue, & *receivedProcess, sizeof(*receivedProcess), 0, !IPC_NOWAIT);

    if (rec_val == -1 )
    {
        // also check if message is empty
        //printf("process not received\n");
        return false;
    }
    //printf("process received\n");
    //printf("ID %d  %d  %d  %d\n",receivedProcess->id,receivedProcess->arrivaltime,receivedProcess->runningtime,receivedProcess->priority );        
    return true;
}
int getPriority(int algorithmNumber, process *receivedProcess)
{
    int chpf=0,csrtn=0,crr=0;
    switch(algorithmNumber){
        case 1:
        chpf=1;
        break;
        case 2:
        csrtn=1;
        break;
        case 3:
        crr=1;
        break;
    }
    int priority=chpf*receivedProcess->priority+csrtn*receivedProcess->remainingtime+crr*receivedProcess->arrivaltime;
    return priority;
}

int main(int argc, char *argv[])
{
    initClk();
    int rec_val;
    struct PriorityQueue *queue = createPQ(atoi(argv[1]));
    key_t ProcessQueue = getProcessQueue();
    while(true){
        struct processData *receivedProcess =malloc(sizeof(struct processData));;
        bool received=receiveProcess(ProcessQueue,receivedProcess);
        //printf("recieved at clock =  %d\n",getClk());
        if(received == true){
            CreateProcessChild(receivedProcess->runningtime);
            //printf("creating at clock =  %d\n",getClk());
            //printf("%d  %d  %d  %d\n",receivedProcess->id,receivedProcess->arrivaltime,receivedProcess->runningtime,receivedProcess->priority );        
        }
    }
    


    // TODO implement the scheduler :)
    // upon termination release the clock resources

    //destroyClk(true);



    //int algorithmNumber=atoi(argv[1]);
    // printf(" algorithm number recieved is %d", algorithmNumber );
    // create empty priority queue
    // enqueue process in priority queue
    //int priority=getPriority(algorithmNumber,receivedProcess);
    // struct CircularQueue *ReadyQueue= createQueue();
    // set priority and set priority queue data with recieved data

    //destroyClk(true);
}
