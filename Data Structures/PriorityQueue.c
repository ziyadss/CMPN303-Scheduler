#pragma once

#include "../Starter Code/headers.h"

size_t parent(const size_t i) { return (i - 1) / 2; }
size_t left(const size_t i) { return 2 * i + 1; }
size_t right(const size_t i) { return 2 * i + 2; }

struct PriorityNode
{
    void *data;
    u_int8_t priority;
};

struct PriorityQueue
{
    struct PriorityNode **nodes;
    size_t size;
    size_t capacity;
};

/**
 * @brief 
 * 
 * @param queue 
 * @param i 
 */
void minHeapify(struct PriorityQueue *queue, size_t i)
{
    size_t l = left(i), r = right(i);

    size_t smallest = i;
    if (l < queue->size && queue->nodes[l]->priority < queue->nodes[i]->priority)
        smallest = l;

    if (r < queue->size && queue->nodes[r]->priority < queue->nodes[i]->priority)
        smallest = r;

    if (smallest != i)
    {
        struct PriorityNode *tmp = queue->nodes[i];
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
int decreasePriority(struct PriorityQueue *queue, size_t i, u_int8_t priority)
{
    if (priority > queue->nodes[i]->priority)
        return -1;

    queue->nodes[i]->priority = priority;

    while (i > 0 && queue->nodes[parent(i)]->priority > queue->nodes[i]->priority)
    {
        struct PriorityNode *tmp = queue->nodes[i];
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
struct PriorityQueue *createPQ(size_t capacity)
{
    struct PriorityQueue *queue = malloc(sizeof(*queue));
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
int enqueuePQ(struct PriorityQueue *queue, void *data, u_int8_t priority)
{
    if (queue->size == queue->capacity)
        return -1;

    struct PriorityNode *node = malloc(sizeof(*node));
    node->data = data;
    node->priority = __UINT8_MAX__;

    queue->nodes[queue->size++] = node;
    return decreasePriority(queue, queue->size - 1, priority);
}

/**
 * @brief 
 * 
 * @param queue 
 * @return void* 
 */
void *peekPQ(struct PriorityQueue *queue)
{
    return (queue->size) ? queue->nodes[0]->data : NULL;
}

/**
 * @brief 
 * 
 * @param queue 
 * @return void* 
 */
void *dequeuePQ(struct PriorityQueue *queue)
{
    if (queue->size == 0)
        return NULL;

    struct PriorityNode *min = queue->nodes[0];
    void *data = min->data;

    queue->nodes[0] = queue->nodes[--queue->size];
    minHeapify(queue, 0);

    free(min);
    return data;
}

/**
 * @brief 
 * 
 * @param queue 
 * @return int 
 */
int freePQ(struct PriorityQueue *queue)
{
    while (queue->size)
        dequeuePQ(queue);

    queue->capacity = 0;
    free(queue->nodes);
    free(queue);
    return 1;
}

// int main()
// {
//     struct PriorityQueue *queue = createPQ(3);

//     int x = 1, y = 2, z = 3;

//     enqueuePQ(queue, &x, 3);
//     enqueuePQ(queue, &y, 2);
//     enqueuePQ(queue, &z, 1);

//     printf("%d\n", *(int *)dequeuePQ(queue));
//     printf("%d\n", *(int *)dequeuePQ(queue));
//     printf("%d\n", *(int *)dequeuePQ(queue));

//     //3 2 1

//     return 0;
// }
