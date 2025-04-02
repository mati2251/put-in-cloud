#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define THREAD_NUM 5

typedef struct node {
  int value;
  struct node *next;
} node_t;

typedef struct list {
  node_t *head;
} list_t;

void list_init(list_t *list) { list->head = NULL; }

void list_insert(list_t *list, int value) {
  node_t *new_node = (node_t *)malloc(sizeof(node_t));
  new_node->value = value;
  new_node->next = NULL;
  node_t *current = list->head;
  if (current == NULL) {
    list->head = new_node;
    return;
  }
  node_t *next = current->next;
  if (current->value > value) {
    new_node->next = current;
    list->head = new_node;
    return;
  }
  while (next != NULL) {
    if (next->value > value) {
      current->next = new_node;
      new_node->next = next;
      return;
    }
    current = next;
    next = next->next;
  }
  current->next = new_node;
}

void list_delete(list_t *list, int value) {
  node_t *current = list->head;
  if (current == NULL) {
    return;
  }
  node_t *next = current->next;
  if (current->value == value) {
    list->head = next;
    free(current);
    return;
  }
  while (next != NULL) {
    if (next->value == value) {
      current->next = next->next;
      free(next);
      return;
    }
    current = next;
    next = next->next;
  }
}

int list_search(list_t *list, int value) {
  node_t *current = list->head;
  while (current != NULL) {
    if (current->value == value) {
      return 1;
    }
    current = current->next;
  }
  return 0;
}

int list_deinit(list_t *list) {
  node_t *current = list->head;
  while (current != NULL) {
    node_t *next = current->next;
    free(current);
    current = next;
  }
  list->head = NULL;
  return 0;
}

int list_count(list_t *list) {
  int count = 0;
  node_t *current = list->head;
  while (current != NULL) {
    count++;
    current = current->next;
  }
  return count;
}

list_t list;

void *thread(void *arg) {
  srand(time(NULL));
  for (int i = 0; i < 1000; i++) {
    int value = rand() % 100000;
    list_insert(&list, i);
  }
  return NULL;
}

int main() {
  list_init(&list);
  pthread_t t[THREAD_NUM];
  for (int i = 0; i < THREAD_NUM; i++) {
    pthread_create(&t[i], NULL, thread, NULL);
  }
  for (int i = 0; i < THREAD_NUM; i++) {
    pthread_join(t[i], NULL);
  }
  printf("%d", list_count(&list));
  list_deinit(&list);
  return 0;
}
