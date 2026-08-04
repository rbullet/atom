#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>

#include "util/collection/list.h"

/**
 * @defgroup util Util
 * @{
 */

/**
 * @defgroup collections Collections
 * @{
 */

/**
 * @defgroup sorted_list Sorted List
 * @brief Intrusive sorted doubly linked list container.
 *
 * A sorted list maintains its elements ordered according to a user-provided
 * comparator function.
 *
 * Sorted lists do not allocate memory and do not own their elements. Nodes
 * must be embedded inside user-defined structures and remain valid while they
 * are stored in the list.
 *
 * Example:
 *
 * @code
 * typedef struct task_t
 * {
 *     uint32_t deadline;
 *     list_node_t node;
 * } task_t;
 *
 * bool task_before(list_node_t const* left, list_node_t const* right);
 *
 * sorted_list_t tasks = SORTED_LIST_INITIALIZER(task_before);
 * @endcode
 *
 * @ingroup collections
 * @{
 */


/**
 * @brief Sorted list comparison function.
 *
 * Determines the ordering between two nodes.
 *
 * The comparator must return true when @p left must appear before @p right
 * in the sorted list.
 *
 * Example:
 *
 * @code
 * return left->deadline < right->deadline;
 * @endcode
 *
 * @param left First node to compare.
 * @param right Second node to compare.
 *
 * @retval true  If left should be ordered before right.
 * @retval false Otherwise.
 */
typedef bool (*sorted_list_comparator_t)(list_node_t const* left, list_node_t const* right);


/**
 * @brief Sorted intrusive list.
 *
 * The list stores pointers to nodes embedded in user objects.
 *
 * The comparator is used by sorted_list_add() to determine the insertion
 * position.
 *
 * The internal fields must not be modified directly while the list is in use.
 */
typedef struct sorted_list_t
{
  list_node_t* head;                    ///< First node in the list.
  list_node_t* tail;                    ///< Last node in the list.
  sorted_list_comparator_t comparator;  ///< Ordering function.
} sorted_list_t;


/**
 * @brief Static sorted list initializer.
 *
 * Creates an empty sorted list using the provided comparator.
 *
 * Example:
 *
 * @code
 * sorted_list_t timers = SORTED_LIST_INITIALIZER(timer_before);
 * @endcode
 */
#define SORTED_LIST_INITIALIZER(c) \
  ((sorted_list_t){ \
    .head = NULL, \
    .tail = NULL, \
    .comparator = (c) \
  })


/**
 * @brief Check whether a sorted list is empty.
 *
 * @param list Sorted list to check.
 *
 * @retval true  If the list contains no nodes.
 * @retval false Otherwise.
 */
static inline bool sorted_list_is_empty(sorted_list_t const* list)
{
  return list_is_empty((list_t*)list);
}


/**
 * @brief Insert a node while maintaining sorted order.
 *
 * The node is inserted at the first position where the comparator indicates
 * that it should appear before the next element.
 *
 * The list does not take ownership of the node.
 *
 * @param list Sorted list to modify.
 * @param node Node to insert.
 *
 * @pre list must be initialized.
 * @pre node must not already belong to another list.
 */
void sorted_list_add(sorted_list_t* list, list_node_t* node);


/**
 * @brief Remove a node from a sorted list.
 *
 * The node is detached from the list but is not destroyed.
 *
 * @param list Sorted list containing the node.
 * @param node Node to remove.
 *
 * @pre node must currently belong to list.
 */
static inline void sorted_list_remove(sorted_list_t* list, list_node_t* node)
{
  list_remove((list_t*)list, node);
}


/** @} */ /* end of sorted_list */
/** @} */ /* end of collection */
/** @} */ /* end of util */


#ifdef __cplusplus
}
#endif