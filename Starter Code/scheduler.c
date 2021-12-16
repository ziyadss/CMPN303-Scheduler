#include "headers.h"
#include "../Data Structures/PriorityQueue.c"
#include "../Data Structures/CircularQueue.c"

void CreateProcessChild(int remainingtime){
    int pidProcess, stat_loc_Process;
    char * arrremainingtime=convertIntegerToChar(remainingtime);
    pidProcess = fork();
        if (pidProcess == 0)
        { //2nd child: forked process
            char *argsProcess[] = {"./process.out",arrremainingtime,NULL};
            execv(argsProcess[0], argsProcess);
        }
        else
        { // parent code: scheduler
            kill(SIGSTP,pidProcess);
        }
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
    //printf("ID %d  %d  %d  %d\n",receivedProcess->id,receivedProcess->arrivaltime,receivedProcess->remainingtime,receivedProcess->priority );        
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
        process *receivedProcess =malloc(sizeof(process));;
        bool received=receiveProcess(ProcessQueue,receivedProcess);
        //printf("recieved at clock =  %d\n",getClk());
        if(received == true){
            CreateProcessChild(receivedProcess->remainingtime);
            //printf("creating at clock =  %d\n",getClk());
            //printf("%d  %d  %d  %d\n",receivedProcess->id,receivedProcess->arrivaltime,receivedProcess->remainingtime,receivedProcess->priority );        
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
