#include <stddef.h>

struct CircularNode
{
    void *data;
    struct CircularNode *prev, *next;
};

struct CircularQueue
{
    struct CircularNode *first;
};

struct CircularQueue *createQueue()
{
    struct CircularQueue *queue = malloc(sizeof(struct CircularQueue));
    queue->first = NULL;
    return queue;
}

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

void *peek(struct CircularQueue *queue)
{
    return (queue->first) ? queue->first->data : NULL;
}

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

void freeQueue(struct CircularQueue *queue)
{
    free(queue);
}