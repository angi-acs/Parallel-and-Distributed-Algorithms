#ifndef QUEUE_H_
#define QUEUE_H_

#include <stdio.h>
#include <stdlib.h>

typedef struct QueueNode {
	Item elem;
	struct QueueNode *next;
} QueueNode;

typedef struct Queue {
	QueueNode *front;
	QueueNode *rear;
	long size;
} Queue;

Queue* createQueue(void) {
	Queue* q = malloc(sizeof(Queue));
	q->front = NULL;
	q->rear = NULL;
	q->size = 0;
	return q;
} 

int isQueueEmpty(Queue *q) {
	if (q->front == NULL && q->rear == NULL)
		return 1;
	else
		return 0;
}

void enqueue(Queue *q, Item elem) {
	QueueNode* node = malloc(sizeof(QueueNode));
	node->elem = elem;
	node->next = NULL;

	if (q->size == 0) {
		q->front = node;
		q->rear = node;
		q->size++;
		return;
	}
	q->rear->next = node;
	q->rear = node;
	q->size++;
}

Item dequeue(Queue* q) {
	Item elem = q->front->elem;

	if (q->size == 1) {
		q->front = NULL;
		q->rear = NULL;
		q->size--;
		return elem;
	}
	q->front = q->front->next;
	q->size--;
	return elem;
}

void destroyQueue(Queue *q) {
	QueueNode* aux = q->front;

	while (aux != NULL) {
		QueueNode* temp = aux->next;
		free(aux);
		aux = temp;
	}
	free(q);
}

#endif
