#pragma once

#include "headers.h"

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
        node->prev->next = node->next;
        node->next->prev = node->prev;
        queue->first = node->next;
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

bool isEmptyCQ(CircularQueue *queue)
{
    return queue->first == NULL;
}

// int main()
// {
//     CircularQueue *queue = createCQ();

//     process a = {1, 1, 1, 1, 1};
//     process b = {2, 2, 2, 2, 2};
//     process c = {3, 3, 3, 3, 3};
//     process d = {4, 4, 4, 4, 4};
//     process e = {5, 5, 5, 5, 5};

//     enqueueCQ(queue, &e);
//     enqueueCQ(queue, &d);
//     enqueueCQ(queue, &c);
//     enqueueCQ(queue, &b);
//     // enqueueCQ(queue, &a);

//     process *p = dequeueCQ(queue);
//     enqueueCQ(queue, &a);
//     enqueueCQ(queue, p);

//     for (int i = 0; i < 10; i++)
//     {
//         process *x = dequeueCQ(queue);
//         printf("%d\n", x->id);
//         enqueueCQ(queue, x);
//     }

//     return 0;
// }
