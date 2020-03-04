#ifndef QUEUE_H
#define QUEUE_H

#include <stdio.h>

#define QUEUEMAXSIZE 50

#define QUEUE_FAILURE -1
#define QUEUE_SUCCESS 0

typedef struct queue_node_s{
    void* payload;
} queue_node;

typedef struct queue_s{
    queue_node* array;
    int front;
    int rear;
    int maxSize;
} queue;
int queue_init(queue* q, int size);
int queue_is_empty(queue* q);
int queue_is_full(queue* q);
int queue_push(queue* q, void* payload);
void* queue_pop(queue* q);
void queue_cleanup(queue* q);

#endif