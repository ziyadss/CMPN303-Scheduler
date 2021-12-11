#pragma once

#include "../Starter Code/headers.h"

// TODO: rename functions to avoid conflict with PQueue
// TODO: use buffers, return -1 for failures and errors

struct CircularNode
{
    void *data;
    struct CircularNode *prev, *next;
};

struct CircularQueue
{
    struct CircularNode *first;
};

/**
 * @brief Create a CicularQueue object
 * 
 * @return struct CircularQueue* 
 */
struct CircularQueue *createQueue()
{
    struct CircularQueue *queue = malloc(sizeof(struct CircularQueue));
    queue->first = NULL;
    return queue;
}

/**
 * @brief Enqueues data into the queue passed
 * 
 * @param queue 
 * @param data 
 */
void enqueue(struct CircularQueue *queue, void *data)
{
    struct CircularNode *node = malloc(sizeof(struct CircularNode));
    node->data = data;

    if (queue->first)
    {
        node->prev = queue->first->prev;
        node->prev->next = node;
        node->next = queue->first;
        queue->first->prev = node;
    }
    else
    {
        node->prev = node;
        node->next = node;
        queue->first = node;
    }
}

/**
 * @brief Returns data from the first node in a queue
 * 
 * @param queue 
 * @return void* 
 */
void *peek(struct CircularQueue *queue)
{
    return (queue->first) ? queue->first->data : NULL;
}

/**
 * @brief Removed the first node in a queue and returns its data
 * 
 * @param queue 
 * @return void* 
 */
void *dequeue(struct CircularQueue *queue)
{
    if (queue->first == NULL)
        return NULL;

    struct CircularNode *node = queue->first;
    void *data = node->data;

    if (node == node->next)
        queue->first = NULL;
    else
    {
        queue->first = node->next;
        queue->first->prev = node->prev;
    }

    free(node);
    return data;
}

/**
 * @brief Frees up memory allocated for a queue
 * 
 * @param queue 
 */
void freeQueue(struct CircularQueue *queue)
{
    while (queue->first)
        dequeue(queue);

    free(queue);
}

// int main()
// {
//     struct CircularQueue *queue = createQueue();

//     int x = 1, y = 2, z = 3;
//     int *xa = &x, *ya = &y, *za = &z;

//     enqueue(queue, xa);
//     enqueue(queue, ya);
//     enqueue(queue, za);

//     printf("%d\n", *(int *)dequeue(queue));
//     printf("%d\n", *(int *)dequeue(queue));
//     printf("%d\n", *(int *)dequeue(queue));

//     return 0;
// }
