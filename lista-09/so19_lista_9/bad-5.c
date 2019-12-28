#include "csapp.h"
#include "queue.h"

typedef struct node {
  SLIST_ENTRY(node) link;
  long value;
} node_t;

typedef SLIST_HEAD(, node) list_t;

node_t *new_node(long value) {
  node_t *node = malloc(sizeof(node_t));
  node->value = value;
  node->link.sle_next = NULL;
  return node;
}

#define N 10

int main(void) {
  list_t head;
  SLIST_INIT(&head);
  for (int i = 0; i < N; i++)
  {
    node_t *new = new_node(i);
    SLIST_INSERT_HEAD(&head, new, link);
  }
  node_t *iter_node;
  SLIST_FOREACH(iter_node, &head, link)
    printf("%p %ld\n", iter_node, iter_node->value);
  while(!SLIST_EMPTY(&head))
  {
    fprintf(stderr, "remove head.slh_first on %p\n", head.slh_first);
    node_t *old_head = SLIST_FIRST(&head);
    SLIST_REMOVE_HEAD(&head, link);
    free(old_head);
  }
  free(SLIST_FIRST(&head));
  return 0;
}
