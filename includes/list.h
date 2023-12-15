#ifndef LIST_H
#define LIST_H

typedef struct node {
  char name[12];
  struct node *next;
} node_t;

node_t *create_node(char *name);

void push(node_t *head, char *name);
int remove_last(node_t *head);
void display_list(node_t *head);
void print_list(node_t *head, char delim);

#endif
