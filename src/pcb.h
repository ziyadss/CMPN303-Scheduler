#pragma once

#include "headers.h"

typedef struct Pcb
{
    int uid;
    int wait_time;
    int burst_time;
    int state;
    int arrival_time;
    int remaining_time;
    int last_run_time;
    int total_time;
    int memory_location;
} Pcb;

typedef struct Process_Table
{
    int capacity;
    int size;
    Pcb **table;
} Process_Table;

//create a PT and its associated PCBs

Process_Table *PT_Create(int capacity)
{
    Process_Table *PT = (Process_Table *)malloc(sizeof(Process_Table));
    PT->table = (Pcb **)calloc(capacity, sizeof(Pcb));
    PT->size = 0;
    PT->capacity = capacity;
    return PT;
}

//returns a pointer to the PCB in PT (hash table indexing, zero indexed)
Pcb *PT_find(Process_Table *PT, process *p)
{
    if (p->id <= 0 || p->id > PT->capacity)
        return NULL;

    return PT->table[p->id - 1];
}
//returns false if: table does not exist or is full, or if pcb already exists
bool PT_insert(Process_Table *PT, process *p)
{
    if (!PT)
        return false;
    else if (PT_find(PT, p))
        return false; //already in table
    else if (PT->size >= PT->capacity)
        return false;

    //process not found

    PT->table[p->id - 1] = (Pcb *)calloc(1, sizeof(Pcb));
    Pcb *PCB = PT->table[p->id - 1];
    PCB->uid = p->id;
    PCB->arrival_time = p->arrivaltime;
    PCB->last_run_time = p->arrivaltime;
    PCB->remaining_time = p->remainingtime;
    PCB->total_time = p->remainingtime;
    PCB->memory_location = -1;
    PT->size++;
    return true;
}

//Frees PCB entry from array of PCB pointers in PT
bool PT_remove(Process_Table *PT, process *p)
{
    if (!PT)
        return false;
    if (!PT_find(PT, p))
        return false;
    if (PT->size == 0)
        return false;

    Pcb *trm = PT->table[p->id - 1];
    free(trm);
    PT->size--;
    return true;
}

void PT_Free(Process_Table *PT)
{
    while (PT->size--)
    {
        if (PT->table[PT->size]->uid <= PT->capacity) //check that the uid is within capacity
            free(PT->table[PT->size]);
    }
    free(PT);
}
