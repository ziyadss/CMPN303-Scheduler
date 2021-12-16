#pragma once

#include "../Starter Code/headers.h"

typedef struct CircularNode
{
    process *p;
    struct CircularNode *prev, *next;
} CircularNode;

typedef struct CircularQueue
{
    CircularNode *first;
} CircularQueue;

/**
 * @brief Create a CicularQueue object
 *
 * @return struct CircularQueue*
 */
CircularQueue *createCQ()
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
void enqueueCQ(struct CircularQueue *queue, process *p)
{
    struct CircularNode *node = malloc(sizeof(*node));
    node->p = p;

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
 * @return process*
 */
process *peekCQ(struct CircularQueue *queue)
{
    return (queue->first) ? queue->first->p : NULL;
}

/**
 * @brief Removed the first node in a queue and returns its data
 *
 * @param queue
 * @return process*
 */
process *dequeueCQ(struct CircularQueue *queue)
{
    if (queue->first == NULL)
        return NULL;

    CircularNode *node = queue->first;
    process *data = node->p;

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
