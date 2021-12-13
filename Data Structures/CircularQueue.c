#pragma once

#include "../Starter Code/headers.h"

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
struct CircularQueue *createCQ()
{
    struct CircularQueue *queue = malloc(sizeof(*queue));
    queue->first = NULL;
    return queue;
}

/**
 * @brief Enqueues data into the queue passed
 * 
 * @param queue 
 * @param data 
 */
void enqueueCQ(struct CircularQueue *queue, void *data)
{
    struct CircularNode *node = malloc(sizeof(*node));
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
void *peekCQ(struct CircularQueue *queue)
{
    return (queue->first) ? queue->first->data : NULL;
}

/**
 * @brief Removed the first node in a queue and returns its data
 * 
 * @param queue 
 * @return void* 
 */
void *dequeueCQ(struct CircularQueue *queue)
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
void freeCQ(struct CircularQueue *queue)
{
    while (queue->first)
        dequeueCQ(queue);

    free(queue);
}

// int main()
// {
//     struct CircularQueue *queue = createCQ();

//     int x = 1, y = 2, z = 3;

//     enqueueCQ(queue, &x);
//     enqueueCQ(queue, &y);
//     enqueueCQ(queue, &z);

//     printf("%d\n", *(int *)dequeueCQ(queue));
//     printf("%d\n", *(int *)dequeueCQ(queue));
//     printf("%d\n", *(int *)dequeueCQ(queue));

//     //1 2 3

//     return 0;
// }
