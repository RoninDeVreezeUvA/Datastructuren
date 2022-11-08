#include <stdio.h>
#include <stdlib.h>

#include "queue.h"

#define DEBUG 0
#define debug_print(fmt) \
            do { if (DEBUG) fprintf(stderr, fmt); } while (0)
/*  Bron: https://stackoverflow.com/questions/1644868/
    define-macro-for-debug-printing-in-c */
    
struct queue {
    int *data;
    size_t rear;
    size_t size;

    int num_of_pushes;
    int num_of_pops;
    size_t max_elements;

    size_t capacity;
};

struct queue *queue_init(size_t capacity) {
    struct queue *queue_ptr = malloc(sizeof(struct queue));
    if(queue_ptr == NULL) {
        debug_print("Could not allocate memory for queue struct\n");
        return NULL;
    }
    
    queue_ptr->data = malloc(sizeof(int) * capacity);
    if(queue_ptr->data == NULL) {
        debug_print("Could not allocate memory for queue data\n");
        free(queue_ptr);
        return NULL;
    }

    queue_ptr->rear = 0;
    queue_ptr->size = 0;
    queue_ptr->capacity = capacity;

    queue_ptr->num_of_pushes = 0;
    queue_ptr->num_of_pops = 0;
    queue_ptr->max_elements = 0;

    return queue_ptr;
}

void queue_cleanup(struct queue *q) {
    if(q == NULL) {
        debug_print("Invalid queue struct in queue_cleanup\n");
        return;
    }

    if(q->data == NULL) {
        debug_print("Invalid queue data in queue_cleanup\n");
        free(q);
        return;
    }

    free(q->data); 
    free(q);
}

void queue_stats(const struct queue *q) {
    if(q == NULL) {
        debug_print("Invalid queue struct in queue_stats\n");
        return;
    }

    fprintf(stderr, "stats %d %d %ld\n", 
                    q->num_of_pushes, 
                    q->num_of_pops, 
                    q->max_elements);
}

int queue_push(struct queue *q, int e) {
    if(q == NULL) {
        debug_print("Invalid queue struct in queue_push\n");
        return 1;
    }

    if(e < 0) {
        debug_print("No negative numbers are allowed in the queue\n");
        return 1;
    }

    if(q->size == q->capacity) {
        debug_print("Queue is full, element not added\n");
        return 1;
    }

    size_t front = (q->rear + q->size) % q->capacity;

    q->data[front] = e;
    q->size++;
    q->num_of_pushes++;

    if(q->size > q->max_elements) {
        q->max_elements = q->size;
    }

    return 0;
}

int queue_pop(struct queue *q) {
    if(q == NULL) {
        debug_print("Invalid queue struct in queue_pop\n");
        return -1;
    }

    if(q->size <= 0) {
        debug_print("Queue is empty, can't pop element\n");
        return -1;
    }

    int element = q->data[q->rear];
    q->rear++;
    q->size--;
    q->num_of_pops++;

    if(q->rear >= q->capacity) {
        q->rear = 0;
    }

    return element;
}

int queue_peek(const struct queue *q) {
    if(q == NULL) {
        debug_print("Invalid queue struct in queue_peek\n");
        return -1;
    }

    if(q->size <= 0) {
        debug_print("Queue is empty, can't peek element\n");
        return -1;
    }

    return q->data[q->rear];
}

int queue_empty(const struct queue *q) {
   if(q == NULL) {
        debug_print("Invalid queue struct in queue_empty\n");
        return -1;
    }

    if(q->size == 0) {
        return 1;
    }

    if(q->size > 0) {
        return 0;
    }

    return -1;
}

size_t queue_size(const struct queue *q) {
    return q->size;
}
