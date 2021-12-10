#include "headers.h"

void clearResources(int);


struct processData
{
    int arrivaltime;
    int priority;
    int runningtime;
    int id;
};


int main(int argc, char *argv[])
{
    signal(SIGINT, clearResources);
    // TODO Initialization
    // 1. Read the input files.
    FILE *pFile;
    char * line = NULL;
    size_t len = 0;
    struct processData pData[10];
    int numberOfProcesses=0;
    ssize_t read;
    pFile = fopen("processes.txt", "r");

    if (pFile == NULL)  // case couldn't read file
    {   
        printf("Error! Could not open file\n"); 
        exit(-1); 
    } 

    int i =0;
    while ((read = getline(&line, &len, pFile)) != -1) {
        if(line[0]!= '#'){
            pData[i].id=atoi(line);
            int numberOfTabs=0;
            bool flag=true;
            char lineCopied[len];
            for(int j=1; j<len ;j++){
                 if(line[j]=='\t'){
                    numberOfTabs++;
                 }
                  if(numberOfTabs==1 && flag==true){ 
                      strncpy(lineCopied, line + j+1, sizeof(line) - j +2);
                      //printf("\nmodified is %s\n",lineCopied);
                      flag=false;
                      pData[i].arrivaltime=atoi(lineCopied);
                  }
                    if(numberOfTabs==2 && flag==false){ 
                        strncpy(lineCopied, line + j+ 1, sizeof(line) - j +2);
                        flag=true;
                        pData[i].runningtime=atoi(lineCopied);
                    }
                    if(numberOfTabs==3 && flag==true){ 
                       strncpy(lineCopied, line + j+ 1, sizeof(line) - j +2);
                       flag=false;
                       pData[i].priority=atoi(lineCopied);
                    }
            }
        printf("%d  %d  %d  %d\n",pData[i].id,pData[i].arrivaltime,pData[i].runningtime,pData[i].priority  );
        i++;
        }
    }
    fclose(pFile);



    // 2. Ask the user for the chosen scheduling algorithm and its parameters, if there are any.
    // 3. Initiate and create the scheduler and clock processes.
    char *argsClk[]={"./clk.out",NULL};
    execv(argsClk[0],argsClk);

    // char *argsScheduler[]={"./scheduler.out",NULL};
    // execv(argsScheduler[0],argsScheduler);



    // 4. Use this function after creating the clock process to initialize clock
    initClk();



    // To get time use this
    int x = getClk();
    printf("current time is %d\n", x);
    // TODO Generation Main Loop
    // 5. Create a data structure for processes and provide it with its parameters.
    // 6. Send the information to the scheduler at the appropriate time.
    // 7. Clear clock resources
    destroyClk(true);
}

void clearResources(int signum)
{
    //TODO Clears all resources in case of interruption
}
