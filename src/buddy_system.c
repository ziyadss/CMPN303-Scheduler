#pragma once

#include "headers.h"

FILE *memoryFile;

struct node
{
    int data;
    int processID;
    int start;
    bool isUsed;
    struct node *left;
    struct node *right;
};

struct node *newNode(int data, int start)
{
    // Allocate memory for new node
    struct node *node = (struct node *)malloc(sizeof(*node));
    node->data = data;
    node->isUsed = false;
    node->start = start;
    node->left = NULL;
    node->right = NULL;
    node->processID = -1;
    return (node);
}

double SizeRounding(double value)
{
    return pow(2, ceil(log2(value)));
}

bool insert(struct node *node, int key, int pid)
{
    bool inserted = false;
    if (node->data == key && node->isUsed == false)
    {
        node->isUsed = true;
        node->processID = pid;
        return true;
    }
    else if (node->data != 2 && node->left == NULL && node->right == NULL && node->data > key)
    {
        node->left = newNode((node->data) / 2, node->start);
        node->right = newNode((node->data) / 2, node->start + (node->data) / 2);
        inserted = insert(node->left, key, pid);
        node->isUsed = inserted;
        if (inserted == false)
            node->isUsed = insert(node->right, key, pid);

        return inserted = node->isUsed;
    }

    return false;
}

void deleteChildren(struct node *parent)
{
    parent->isUsed = false;
    free(parent->left);
    free(parent->right);
    parent->left = NULL;
    parent->right = NULL;
}

void deallocate(struct node *node, int pid)
{
    if (node == NULL)
        return;
    /* first recur on left child */
    deallocate(node->left, pid);
    deallocate(node->right, pid);
    if (node->processID == pid)
    {
        node->isUsed = false;
        node->processID = -1;
        fprintf(memoryFile, "At time %d freed %d bytes from process %d from %d to %d\n",
                getClk(), node->data, pid, node->start, node->start + node->data - 1);
        return;
    }

    if (node->left && node->left->isUsed == false && node->right && node->right->isUsed == false)
        return deleteChildren(node);
}

bool findNode(struct node *node, int size, struct node **ptr, int bestsize)
{
    if (node == NULL)
        return false;
    bool check = false;
    /* first recur on left child */
    check = findNode(node->left, size, ptr, bestsize);
    if (node->left == NULL && node->right == NULL)
    {
        if (node->data == size && node->isUsed == false)
        {
            *ptr = node;
            bestsize = node->data;
            return true;
        }
        else if (node->data > size && node->data < bestsize && node->isUsed == false)
        {
            *ptr = node;
            // printf("%d", **ptr->data);
            bestsize = node->data;
            return true;
        }
        return false;
    }
    /* now recur on right child */
    if (check == false)
        check = findNode(node->right, size, ptr, bestsize);
    return check;
}

bool allocate(struct node *node, int key, int pid)
{
    int number = SizeRounding((double)key);
    struct node *ptr = NULL;
    findNode(node, number, &ptr, 2000);

    bool allocated = ptr && insert(ptr, number, pid);

    if (allocated)
        fprintf(memoryFile, "At time %d allocated %d bytes for process %d from %d to %d\n",
                getClk(), number, pid, ptr->start, ptr->start + number - 1);

    return allocated;
}

void printInorder2(struct node *node)
{
    if (node == NULL)
        return;

    /* first recur on left child */
    printInorder2(node->left);

    /* then print the data of node */
    if (node->left == NULL && node->right == NULL)
        printf("%d %s %d %d\n", node->data, node->isUsed ? "true" : "false", node->start, node->processID);

    /* now recur on right child */
    printInorder2(node->right);
}

int GetStartingIndex(struct node *node, int pid)
{
    if (node == NULL)
        return -1;

    if (pid == node->processID)
        return node->start;

    return max(GetStartingIndex(node->left, pid), GetStartingIndex(node->right, pid));
}

// int main()
// {
//     //create root/
//     memoryFile = fopen("memory.log", "w");
//     struct node *root = newNode(1024, 0);

//     bool i = allocate(root, 116, 6);
//     i = allocate(root, 120, 8);
//     i = allocate(root, 40, 2);
//     // i = allocate(root,240);

//     if (i == true)
//         printf("Allocated!\n");
//     else
//         printf("full\n");

//     printInorder2(root);

//     printf("\nDeallocation\n");
//     deallocate(root, 8);
//     deallocate(root, 6);
//     deallocate(root, 2);

//     printInorder2(root);

//     return 0;
// }