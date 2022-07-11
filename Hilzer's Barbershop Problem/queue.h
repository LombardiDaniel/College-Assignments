#ifndef QUEUE_H
#define QUEUE_H

#include <stdlib.h>

typedef int Tipo;


typedef struct QNode_ {
    Tipo data;
    struct QNode_ *next;
} QNode;

typedef struct Queue_ {
    QNode *front;
    QNode *rear;
} Queue;


void enqueue(Queue *queue, Tipo value);
Tipo dequeue(Queue *queue);
inline int isEmpty(Queue *queue);


void enqueue(Queue *queue, Tipo value) {

    QNode *newNode = (QNode *) malloc(sizeof(QNode));

    newNode->data = value;
    newNode->next = NULL;

    // if it is the first node
    if (queue->front == NULL && queue->rear == NULL) {
        //make both front and rear points to the new node
        queue->front = newNode;
        queue->rear = newNode;
    } else {
        //add newnode in rear->next
        queue->rear->next = newNode;

        //make the new node as the rear node
        queue->rear = newNode;
    }
}


Tipo dequeue(Queue *queue) {
    // used to freeing the first node after dequeue
    QNode *tmp;

    if (queue->front == NULL)
        return NULL;

    // take backup
    tmp = queue->front;

    // make the front node points to the next node
    // logically removing the front element
    queue->front = queue->front->next;

    // if front == NULL, set rear = NULL
    if (queue->front == NULL)
        queue->rear = NULL;

   // free the first node
   Tipo tmpData = tmp->data;
   free(tmp);
   return tmpData;
}

inline int isEmpty(Queue *queue) {
    if (queue->front == NULL)
        return 1;

    return 0;
}

#endif
