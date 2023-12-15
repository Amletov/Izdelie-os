#ifndef LIST_H
#define LIST_H

typedef struct node {
  char name[12];
  struct node *next;
} node_t;

void push(node_t *head, char *name);
int remove_last(node_t *head);
void display_list(node_t *head);
char *list_to_str(node_t *head);

#endif