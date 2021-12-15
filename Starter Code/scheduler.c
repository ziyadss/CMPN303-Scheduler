#include "headers.h"


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

bool receiveProcess(key_t ProcessQueue, struct processData *receivedProcess)
{
    int rec_val;
    rec_val = msgrcv(ProcessQueue, & *receivedProcess, sizeof(*receivedProcess), 0, IPC_NOWAIT);

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
int getPriority(int algorithmNumber, struct processData *receivedProcess)
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
    int priority=chpf*receivedProcess->priority+csrtn*receivedProcess->runningtime+crr*receivedProcess->arrivaltime;
    return priority;
}

int main(int argc, char *argv[])
{
    initClk();
    //printf("recieved at clock =  %d\n",getClk());
    int rec_val;
    key_t ProcessQueue = getProcessQueue();
    while(true){
        struct processData *receivedProcess =malloc(sizeof(struct processData));;
        bool received=receiveProcess(ProcessQueue,receivedProcess);

        if(received == true){
            //printf("recieved at clock =  %d\n",getClk());
            printf("%d  %d  %d  %d\n",receivedProcess->id,receivedProcess->arrivaltime,receivedProcess->runningtime,receivedProcess->priority );        
        }
        sleep(1);
    }
    


    //TODO implement the scheduler :)
    //upon termination release the clock resources

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