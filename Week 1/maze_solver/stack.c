#include <stdio.h>
#include <stdlib.h>

#include "stack.h"

#define DEBUG 0
#define debug_print(fmt) \
            do { if (DEBUG) fprintf(stderr, fmt); } while (0)
/*  Bron: https://stackoverflow.com/questions/1644868/
    define-macro-for-debug-printing-in-c */
    
struct stack {
    int *data;
    size_t size;

    int num_of_pushes;
    int num_of_pops;
    size_t max_elements;

    size_t capacity;
};

struct stack *stack_init(size_t capacity) {
    struct stack* stack_ptr = malloc(sizeof(struct stack)); 
    if(stack_ptr == NULL) {
        debug_print("Could not allocate memory for stack struct\n");
        return NULL;
    }
    
    stack_ptr->data = malloc(sizeof(int) * capacity);
    if(stack_ptr->data == NULL) {
        debug_print("Could not allocate memory for stack data\n");
        free(stack_ptr);
        return NULL;
    }

    stack_ptr->size = 0;
    stack_ptr->capacity = capacity;

    stack_ptr->num_of_pushes = 0;
    stack_ptr->num_of_pops = 0;
    stack_ptr->max_elements = 0;
    return stack_ptr;
}

void stack_cleanup(struct stack *s) {
    if(s == NULL) {
        debug_print("Invalid stack struct in stack_cleanup\n");
        return;
    }

    if(s->data == NULL) {
        debug_print("Invalid stack data in stack_cleanup\n");
        free(s);
        return;
    }

    free(s->data); 
    free(s);
}

void stack_stats(const struct stack *s) {
    if(s == NULL) {
        debug_print("Invalid stack struct in stack_stats\n");
        return;
    }

    fprintf(stderr, "stats %d %d %ld\n", 
                    s->num_of_pushes, 
                    s->num_of_pops, 
                    s->max_elements);
}

int stack_push(struct stack *s, int c) {
    if(s == NULL) {
        debug_print("Invalid stack struct in stack_push\n");
        return 1;
    }

    if(c < 0) {
        debug_print("No negative numbers are allowed on the stack\n");
        return 1;
    }

    if(s->size >= s->capacity) {
        debug_print("Maximum number of elements on stack reached, element not added\n");
        return 1;
    }

    s->data[s->size] = c;
    s->size++;
    s->num_of_pushes++;

    if(s->size > s->max_elements) {
        s->max_elements = s->size;
    }

    return 0;
}

int stack_pop(struct stack *s) {
    if(s == NULL) {
        debug_print("Invalid stack struct in stack_pop\n");
        return -1;
    }

    if(stack_empty(s) != 0) {
        debug_print("Stack is empty, can't pop element\n");
        return -1;
    }

    s->size--;
    s->num_of_pops++;
    return s->data[s->size];
}

int stack_peek(const struct stack *s) {
    if(s == NULL) {
        debug_print("Invalid stack struct in stack_peek\n");
        return -1;
    }

    if(stack_empty(s) != 0) {
        debug_print("Stack is empty, can't peek element\n");
        return -1;
    }

    return s->data[s->size - 1];
}

int stack_empty(const struct stack *s) {
    if(s == NULL) {
        debug_print("Invalid stack struct in stack_empty\n");
        return -1;
    }

    if(s->size == 0) {
        return 1;
    }

    if(s->size > 0) {
        return 0;
    }

    return -1;
}

size_t stack_size(const struct stack *s) {
    return s->size;
}
