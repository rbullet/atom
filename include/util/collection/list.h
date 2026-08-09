#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>

/**
 * @defgroup util Util
 * @{
 */

/**
 * @defgroup collections Collections
 * @brief Generic intrusive collection containers.
 * @{
 */


/**
 * @defgroup list List
 * @brief Intrusive doubly linked list.
 *
 * Provides a lightweight doubly linked list implementation without memory
 * allocation.
 *
 * Lists do not own their elements. Nodes are embedded directly inside user
 * structures and are linked using list_node_t.
 *
 * Example:
 *
 * @code
 * typedef struct task_t
 * {
 *     uint32_t id;
 *     list_node_t node;
 * } task_t;
 *
 * list_t tasks = LIST_INITIALIZER;
 *
 * list_push(&tasks, &task.node);
 * @endcode
 *
 * @ingroup collections
 * @{
 */


/**
 * @brief Linked list node.
 *
 * A node must be embedded inside a user-defined structure.
 *
 * The node must not be inserted into multiple lists simultaneously.
 */
typedef struct list_node_t
{
  struct list_node_t* previous; ///< Previous node.
  struct list_node_t* next;     ///< Next node.
} list_node_t;


/**
 * @brief Doubly linked intrusive list.
 *
 * The list does not allocate or free nodes.
 *
 * Nodes remain owned by the caller and must remain valid while stored in
 * the list.
 */
typedef struct list_t
{
  list_node_t* head; ///< First node in the list.
  list_node_t* tail; ///< Last node in the list.
} list_t;


/**
 * @brief List iterator.
 *
 * Iterators allow traversal and removal while walking a list.
 *
 * Removing the current element using list_iterator_remove() is supported.
 */
typedef struct list_iterator_t
{
  list_node_t* current; ///< Last returned node.
  list_node_t* next;    ///< Next node to return.
  list_t* backend;      ///< List being iterated.
} list_iterator_t;


/**
 * @brief Empty list initializer.
 *
 * Example:
 *
 * @code
 * list_t queue = LIST_INITIALIZER;
 * @endcode
 */
#define LIST_INITIALIZER \
  ((list_t){ \
    .head = NULL, \
    .tail = NULL \
  })


/**
 * @brief Empty node initializer.
 *
 * Initializes a node detached from any list.
 */
#define LIST_NODE_INITIALIZER \
  ((list_node_t){ \
    .previous = NULL, \
    .next = NULL \
  })


/**
 * @brief Empty iterator initializer.
 */
#define EMPTY_LIST_ITERATOR \
  ((list_iterator_t){ \
    .current = NULL, \
    .next = NULL, \
    .backend = NULL \
  })


/**
 * @brief Append a node to a list.
 *
 * Inserts the node at the end of the list.
 *
 * The list does not take ownership of the node.
 *
 * @param list List to modify.
 * @param node Node to insert.
 *
 * @pre node must not already belong to another list.
 */
static inline void list_push(list_t* list, list_node_t* node)
{
  node->next = NULL;
  node->previous = NULL;

  if (list->tail != NULL)
  {
    list->tail->next = node;
    node->previous = list->tail;
    list->tail = node;
  }
  else
  {
    list->head = node;
    list->tail = node;
  }
}


/**
 * @brief Remove a node from a list.
 *
 * Detaches the node from the list but does not destroy it.
 *
 * @param list List containing the node.
 * @param node Node to remove.
 *
 * @pre node must currently belong to list.
 */
static inline void list_remove(list_t* list, list_node_t* node)
{
  if (node->previous != NULL)
  {
    node->previous->next = node->next;
  }
  else
  {
    list->head = node->next;
  }

  if (node->next != NULL)
  {
    node->next->previous = node->previous;
  }
  else
  {
    list->tail = node->previous;
  }

  node->previous = NULL;
  node->next = NULL;
}


/**
 * @brief Remove and return the first node.
 *
 * Removes the head element from the list.
 *
 * @param list List to modify.
 *
 * @return Removed node, or NULL if the list is empty.
 */
static inline list_node_t* list_pop(list_t* list)
{
  list_node_t* node = list->head;

  if (node == NULL)
  {
    return NULL;
  }

  list->head = node->next;

  if (list->head != NULL)
  {
    list->head->previous = NULL;
  }
  else
  {
    list->tail = NULL;
  }

  node->previous = NULL;
  node->next = NULL;

  return node;
}


/**
 * @brief Check whether a list is empty.
 *
 * @param list List to check.
 *
 * @retval true  List contains no nodes.
 * @retval false List contains at least one node.
 */
static inline bool list_is_empty(list_t const* list)
{
  return list->head == NULL;
}

/**
 * @brief Check whether a node belongs to a list.
 *
 * The comparison is based on node identity, not on the comparator.
 *
 * @param list List to inspect.
 * @param node Node to search for.
 *
 * @retval true  If node is contained in the list.
 * @retval false Otherwise.
 */
static inline bool list_contains(list_t const* list, list_node_t const* node)
{
  list_node_t const* current = list->head;

  while (current != NULL)
  {
    if (current == node)
    {
      return true;
    }
    current = current->next;
  }

  return false;
}


/**
 * @brief Create a list iterator.
 *
 * Example:
 *
 * @code
 * list_iterator_t it = LIST_ITERATOR_CREATE(&tasks);
 *
 * while (list_iterator_has_next(&it))
 * {
 *     list_node_t* node = list_iterator_next(&it);
 * }
 * @endcode
 */
#define LIST_ITERATOR_CREATE(list_ptr) \
  (list_iterator_t){ \
    .current = NULL, \
    .next = (list_ptr)->head, \
    .backend = (list_ptr) \
  }


/**
 * @brief Check whether another node is available.
 *
 * @param iterator Iterator to check.
 *
 * @retval true  Another node exists.
 * @retval false End of list reached.
 */
static inline bool list_iterator_has_next(list_iterator_t const* iterator)
{
  return iterator->next != NULL;
}


/**
 * @brief Return the next node and advance the iterator.
 *
 * @param iterator Iterator to advance.
 *
 * @return Next node, or NULL when iteration is complete.
 */
static inline list_node_t* list_iterator_next(list_iterator_t* iterator)
{
  iterator->current = iterator->next;

  if (iterator->current != NULL)
  {
    iterator->next = iterator->current->next;
  }
  else
  {
    iterator->next = NULL;
  }

  return iterator->current;
}


/**
 * @brief Remove the current iterator node.
 *
 * Removes the node returned by the last call to list_iterator_next().
 *
 * @param iterator Iterator currently positioned on a node.
 *
 * @pre list_iterator_next() must have returned a valid node.
 */
static inline void list_iterator_remove(list_iterator_t* iterator)
{
  list_remove(iterator->backend, iterator->current);
  iterator->current = NULL;
}


/** @} */ /* end of list */
/** @} */ /* end of collection */
/** @} */ /* end of util */


#ifdef __cplusplus
}
#endif