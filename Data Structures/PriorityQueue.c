#pragma once

#include "../Starter Code/headers.h"

size_t parent(const size_t i) { return (i - 1) / 2; }
size_t left(const size_t i) { return 2 * i + 1; }
size_t right(const size_t i) { return 2 * i + 2; }

typedef struct PriorityQueue
{
    size_t size;
    size_t capacity;
    process **nodes;
} PriorityQueue;

/**
 * @brief 
 * 
 * @param queue 
 * @param a 
 * @param b 
 * @return int 
 */
int processSwap(PriorityQueue *queue, size_t a, size_t b)
{
    process *tmp = queue->nodes[a];
    queue->nodes[a] = queue->nodes[b];
    queue->nodes[b] = tmp;
    return a;
}

/**
 * @brief
 *
 * @param queue
 * @param i
 */
void minHeapify(PriorityQueue *queue, size_t i)
{
    size_t l = left(i), r = right(i);

    size_t smallest = i;
    if (l < queue->size && queue->nodes[l]->priority < queue->nodes[smallest]->priority)
        smallest = l;

    if (r < queue->size && queue->nodes[r]->priority < queue->nodes[smallest]->priority)
        smallest = r;

    if (smallest != i)
    {
        processSwap(queue, i, smallest);
        minHeapify(queue, smallest);
    }
};

/**
 * @brief
 *
 * @param queue
 * @param i
 * @param priority
 * @return int
 */
int decreasePriority(PriorityQueue *queue, size_t i, u_int8_t priority)
{
    if (priority > queue->nodes[i]->priority)
        return -1;

    queue->nodes[i]->priority = priority;

    while (i > 0 && queue->nodes[parent(i)]->priority > queue->nodes[i]->priority)
    {
        i = processSwap(queue, i, parent(i));
    }
    return 0;
}

/**
 * @brief
 *
 * @param capacity
 * @return struct PriorityQueue*
 */
PriorityQueue *createPQ(size_t capacity)
{
    PriorityQueue *queue = malloc(sizeof(*queue));
    queue->nodes = malloc(sizeof(*(queue->nodes)) * capacity);
    queue->size = 0;
    queue->capacity = capacity;

    return queue;
}

/**
 * @brief
 *
 * @param queue
 * @param data
 * @param priority
 * @return int
 */
int enqueuePQ(PriorityQueue *queue, process *node)
{
    if (queue->size == queue->capacity)
        return -1;

    queue->nodes[queue->size++] = node;
    return decreasePriority(queue, queue->size - 1, node->priority);
}

/**
 * @brief
 *
 * @param queue
 * @return process*
 */
process *peekPQ(PriorityQueue *queue)
{
    return (queue->size) ? queue->nodes[0] : NULL;
}

/**
 * @brief
 *
 * @param queue
 * @return process*
 */
process *dequeuePQ(PriorityQueue *queue)
{
    if (queue->size == 0)
        return NULL;

    process *min = queue->nodes[0];

    queue->nodes[0] = queue->nodes[--queue->size];
    minHeapify(queue, 0);
    return min;
}

/**
 * @brief
 *
 * @param queue
 * @return int
 */
int freePQ(PriorityQueue *queue)
{
    queue->capacity = 0;
    free(queue->nodes);
    free(queue);
    return 1;
}

// int main()
// {
//     PriorityQueue *queue = createPQ(5);

//     process *z = createProcess(2, 4, 10, 1);
//     process *x = createProcess(2, 2, 5, 2);
//     process *y = createProcess(8, 1, 8, 3);
//     process *b = createProcess(10, 6, 25, 4);
//     process *v = createProcess(12, 5, 25, 5);

//     enqueuePQ(queue, x);
//     enqueuePQ(queue, y);
//     enqueuePQ(queue, z);
//     enqueuePQ(queue, b);
//     enqueuePQ(queue, v);

//     while (peekPQ(queue))
//     {
//         process *d = dequeuePQ(queue);
//         printf("%d\t %d \n", d->id, d->priority);
//     }

//     /*
//         3        1
//         2        2
//         1        4
//         5        5
//         4        6
//     */

//     return 0;
// }
