#include "list.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>

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

char *list_to_str(node_t *head) {
  
  node_t *current = head;
  char *list;
  while (current->next != NULL) {
    strcat(list, current->name);
    strcat(list, "/");
    current = current->next;
  }   

  return list;
} 