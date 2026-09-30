#include "../includes/list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

node_t *create_node(char *name) {
  node_t *n = malloc(sizeof(node_t));
  strcpy(n->name, name);
  n->next = NULL;
  return n;
}

void push(node_t *head, char *name) {
  node_t *current = head;
  while (current->next != NULL) {
    current = current->next;
  }

  current->next = (node_t *)malloc(sizeof(node_t));
  strcpy(current->next->name, name);
  current->next->next = NULL;
}

int remove_last(node_t *head) {
  char *retval;

  if (head->next == NULL) {
    retval = head->name;
    free(head);
    return -1;
  }

  node_t *current = head;
  while (current->next->next != NULL) {
    current = current->next;
  }

  strcpy(retval, current->next->name);
  free(current->next);
  current->next = NULL;
  return 0;
}

void display_list(node_t *head) {
  node_t *current = head;
  while (current != NULL) {
    printf("%s/", current->name);
    current = current->next;
  }
}

void print_list(node_t *head, char delim) {

  node_t *current = head;
  while (current != NULL) {

    printf("%s%c", current->name, delim);
    current = current->next;
  }
}
