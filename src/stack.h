#ifndef STACK_H
#define STACK_H

typedef struct node {
    unsigned val;
    struct node* next;
} node;

struct stack {
    node head;
};

static void push(struct stack* s, unsigned v) {
    node* new_node = (node*)malloc(sizeof(node));
    new_node->val = v;
    new_node->next = s->head.next;
    s->head.next = new_node;
}

static unsigned top(struct stack* s) {
    return s->head.next->val;
}

static void pop(struct stack* s) {
    node* to_delete = s->head.next;
    s->head.next = to_delete->next;
    free(to_delete);
}

#endif