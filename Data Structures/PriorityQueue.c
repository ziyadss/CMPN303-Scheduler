#include <stdlib.h>

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

struct PriorityQueue *createPriorityQueue(size_t capacity)
{
    struct PriorityQueue *queue = malloc(sizeof(struct PriorityQueue));
    queue->nodes = malloc(sizeof(struct PriorityNode *) * capacity);
    queue->size = 0;
    queue->capacity = capacity;

    return queue;
}

void decreasePriority(struct PriorityQueue *queue, size_t i, u_int8_t priority)
{
    if (priority > queue->nodes[i]->priority)
        return;

    queue->nodes[i]->priority = priority;

    while (i > 0 && queue->nodes[parent(i)]->priority > queue->nodes[i]->priority)
    {
        struct PriorityNode *tmp = queue->nodes[i];
        queue->nodes[i] = queue->nodes[parent(i)];
        queue->nodes[parent(i)] = tmp;
        i = parent(i);
    }
}

void enqueue(struct PriorityQueue *queue, void *data, u_int8_t priority)
{
    struct PriorityNode *node = malloc(sizeof(struct PriorityNode));
    node->data = data;
    node->priority = __UINT8_MAX__;

    queue->nodes[queue->size++] = node;
    decreasePriority(queue, queue->size - 1, priority);
}

struct PriorityNode *peek(struct PriorityQueue *queue)
{
    return (queue->size) ? queue->nodes[0] : NULL;
}

void *dequeue(struct PriorityQueue *queue)
{
    struct PriorityNode *min = peek(queue);
    if (min == NULL)
        return NULL;

    void *data = min->data;

    queue->nodes[0] = queue->nodes[--queue->size];
    minHeapify(queue, 0);

    free(min);
    return data;
}

void freePriorityQueue(struct PriorityQueue *queue)
{
    free(queue->nodes);
    free(queue);
}