#pragma once

#ifdef __cplusplus
extern "C" {

#endif

#include "concurrent/spinlock.h"
#include "util/collection/list.h"
#include "util/collection/ring_buffer.h"

/**
 * @defgroup concurrent Concurrent
 * @{
 */

/**
 * @defgroup message_queue Message Queue
 * @brief Inter-thread message passing.
 *
 * Message queues provide a thread-safe mechanism for passing fixed-size
 * messages between threads. Messages are stored in an application-provided
 * ring buffer.
 *
 * @{
 */

typedef struct
{
  ring_buffer_t buffer;
  spinlock_t spinlock;
  list_t receivers;
  list_t senders;
} message_queue_t;


#define MESSAGE_QUEUE_INITIALIZER(storage, msg_size, cap) \
  ((message_queue_t){ \
    .buffer = RING_BUFFER_INITIALIZER((storage), (msg_size), (cap)), \
    .spinlock = SPINLOCK_INITIALIZER, \
    .receivers = LIST_INITIALIZER, \
    .senders = LIST_INITIALIZER \
  })

/**
 * @brief Tries to push a message onto the message queue.
 *
 * @param message_queue Message queue to push the message onto.
 * @param message Message to push onto the queue.
 *
 * @return true if the message was successfully pushed, otherwise false.
 */
bool message_queue_try_push(message_queue_t* message_queue, void const* message);


/**
 * @brief Pushes a message onto the message queue, blocking if the queue is full.
 *
 * @param message_queue Message queue to push the message onto.
 * @param message Message to push onto the queue.
 */
void message_queue_push(message_queue_t* message_queue, void const* message);


/**
 * @brief Pushes a message onto the message queue, blocking if the queue is full, with a timeout.
 *
 * @param message_queue Message queue to push the message onto.
 * @param message Message to push onto the queue.
 * @param timeout Maximum duration to wait for space in the queue.
 *
 * @return true if the message was successfully pushed, otherwise false if the timeout expired.
 */
bool message_queue_push_with_timeout(message_queue_t* message_queue, void const* message, duration_t timeout);


/**
 * @brief Tries to pop a message from the message queue.
 *
 * @param message_queue Message queue to pop the message from.
 * @param message Pointer to store the popped message.
 *
 * @return true if the message was successfully popped, otherwise false.
 */
bool message_queue_try_pop(message_queue_t* message_queue, void* message);


/**
 * @brief Pops a message from the message queue, blocking if the queue is empty.
 *
 * @param message_queue Message queue to pop the message from.
 * @param message Pointer to store the popped message.
 */
void message_queue_pop(message_queue_t* message_queue, void* message);


/**
 * @brief Pops a message from the message queue, blocking if the queue is empty, with a timeout.
 *
 * @param message_queue Message queue to pop the message from.
 * @param message Pointer to store the popped message.
 * @param timeout Maximum duration to wait for a message in the queue.
 *
 * @return true if the message was successfully popped, otherwise false if the timeout expired.
 */
void message_queue_pop_with_timeout(message_queue_t* message_queue, void* message, duration_t timeout);


/**
 * @brief Checks if the message queue is empty.
 *
 * @param message_queue Message queue to inspect.
 *
 * @return true if the queue is empty, otherwise false.
 */
bool message_queue_is_empty(message_queue_t const* message_queue);


/**
 * @brief Checks if the message queue is full.
 *
 * @param message_queue Message queue to inspect.
 *
 * @return true if the queue is full, otherwise false.
 */
bool message_queue_is_full(message_queue_t const* message_queue);


/**
 * @brief Gets the number of messages currently in the message queue.
 *
 * @param message_queue Message queue to inspect.
 *
 * @return Number of messages in the queue.
 */
size_t message_queue_count(message_queue_t const* message_queue);


/**
 * @brief Gets the capacity of the message queue.
 *
 * @param message_queue Message queue to inspect.
 *
 * @return Capacity of the queue.
 */
size_t message_queue_capacity(message_queue_t const* message_queue);

/** @} */ /* end of message_queue */
/** @} */ /* end of concurrent */

#ifdef __cplusplus
}
#endif
