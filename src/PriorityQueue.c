#pragma once

#include "../src/headers.h"

size_t parent(const size_t i) { return (i - 1) / 2; }
size_t left(const size_t i) { return 2 * i + 1; }
size_t right(const size_t i) { return 2 * i + 2; }

typedef struct PriorityQueue
{
    size_t size;
    size_t capacity;
    process **nodes;
    bool (*compare)(process *, process *);
} PriorityQueue;

bool priorityCompare(process *a, process *b)
{
    return a->priority < b->priority;
}

bool timeCompare(process *a, process *b)
{
    return a->remainingtime < b->remainingtime;
}

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
    if (l < queue->size && queue->compare(queue->nodes[l], queue->nodes[smallest]))
        smallest = l;

    if (r < queue->size && queue->compare(queue->nodes[r], queue->nodes[smallest]))
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

    while (i > 0 && queue->compare(queue->nodes[i], queue->nodes[parent(i)]))
        i = processSwap(queue, parent(i), i);

    return 0;
}

/**
 * @brief
 *
 * @param capacity
 * @return struct PriorityQueue*
 */
PriorityQueue *createPQ(size_t capacity, bool (*predicate)(process *, process *))
{
    PriorityQueue *queue = malloc(sizeof(*queue));
    queue->nodes = malloc(sizeof(*(queue->nodes)) * capacity);
    // queue->nodes = calloc(capacity, sizeof(*(queue->nodes)));
    queue->size = 0;
    queue->capacity = capacity;
    queue->compare = predicate;

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

bool isEmptyPQ(PriorityQueue *queue)
{
    return queue->size == 0;
}

// int main()
// {
//     PriorityQueue *queue = createPQ(5, timeCompare);

//     process a = {1, 1, 1, 1, 1};
//     process b = {2, 2, 2, 2, 2};
//     process c = {3, 3, 3, 3, 3};
//     process d = {4, 4, 4, 4, 4};
//     process e = {5, 5, 5, 5, 5};

//     enqueuePQ(queue, &e);
//     enqueuePQ(queue, &d);
//     enqueuePQ(queue, &c);
//     enqueuePQ(queue, &b);
//     enqueuePQ(queue, &a);

//     while (peekPQ(queue))
//     {
//         process *x = dequeuePQ(queue);
//         printf("%d\t %d \n", x->id, x->priority);
//     }

//     return 0;
// }
