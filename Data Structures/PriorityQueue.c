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
 * @param i
 */
void minHeapify(PriorityQueue *queue, size_t i)
{
    size_t l = left(i), r = right(i);

    size_t smallest = i;
    if (l < queue->size && queue->nodes[l]->priority < queue->nodes[i]->priority)
        smallest = l;

    if (r < queue->size && queue->nodes[r]->priority < queue->nodes[i]->priority)
        smallest = r;

    if (smallest != i)
    {
        process *tmp = queue->nodes[i];
        queue->nodes[i] = queue->nodes[smallest];
        queue->nodes[smallest] = tmp;
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
        process *tmp = queue->nodes[i];
        queue->nodes[i] = queue->nodes[parent(i)];
        queue->nodes[parent(i)] = tmp;
        i = parent(i);
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
    // free when
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
//     PriorityQueue *queue = createPQ(3);

//     process *x = createProcess(8, 3, 2 , 3);
//     process *y = createProcess(0, 1, 6, 7);
//     process *z = createProcess(4, 2, 10, 11);

//     enqueuePQ(queue, x);
//     enqueuePQ(queue, y);
//     enqueuePQ(queue, z);

//     printf("%d\n", *(int *)dequeuePQ(queue));
//     printf("%d\n", *(int *)dequeuePQ(queue));
//     printf("%d\n", *(int *)dequeuePQ(queue));

//     //3 2 1

//     return 0;
// }
