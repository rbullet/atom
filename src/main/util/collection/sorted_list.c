#include "util/collection/sorted_list.h"

void sorted_list_add(sorted_list_t* list, list_node_t* node)
{
  node->previous = NULL;
  node->next = NULL;

  list_node_t* current = list->head;

  while (current != NULL && !list->comparator(node, current))
  {
    current = current->next;
  }

  if (current == NULL)
  {
    list_push((list_t*)list, node);
    return;
  }

  node->next = current;
  node->previous = current->previous;

  if (current->previous != NULL)
  {
    current->previous->next = node;
  }
  else
  {
    list->head = node;
  }

  current->previous = node;
}
