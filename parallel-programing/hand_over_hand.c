#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#define THREAD_NUM 5

typedef struct node {
  int value;
  struct node *next;
  pthread_mutex_t lock;
} node_t;

typedef struct list {
  node_t *head;
  pthread_mutex_t head_lock;
} list_t;

void list_init(list_t *list) {
  list->head = NULL;
  pthread_mutex_init(&list->head_lock, NULL);
}

void lock(node_t *node) {
  if (node == NULL) {
    return;
  }
  pthread_mutex_lock(&node->lock);
}

void unlock(node_t *node) {
  if (node == NULL) {
    return;
  }
  pthread_mutex_unlock(&node->lock);
}

void list_insert(list_t *list, int value) {
  node_t *new_node = (node_t *)malloc(sizeof(node_t));
  new_node->value = value;
  new_node->next = NULL;
  pthread_mutex_init(&new_node->lock, NULL);

  pthread_mutex_lock(&list->head_lock);
  node_t *current = list->head;
  lock(current);
  if (current == NULL) {
    list->head = new_node;
    unlock(current);
    pthread_mutex_unlock(&list->head_lock);
    return;
  }
  if (current->value > value) {
    new_node->next = current;
    list->head = new_node;
    unlock(current);
    pthread_mutex_unlock(&list->head_lock);
    return;
  }
  pthread_mutex_unlock(&list->head_lock);
  lock(current->next);
  node_t *next = current->next;
  while (next != NULL) {
    if (next->value > value) {
      current->next = new_node;
      new_node->next = next;
      unlock(current);
      unlock(next);
      return;
    }
    node_t *temp = current;
    lock(next->next);
    current = next;
    next = next->next;
    unlock(temp);
  }
  current->next = new_node;
  unlock(current);
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
  printf("%d\n", list_count(&list));
  list_deinit(&list);
  return 0;
}
