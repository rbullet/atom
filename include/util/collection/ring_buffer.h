#pragma once

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stddef.h>
#include <stdbool.h>

/**
 * @defgroup util Util
 * @{
 */

/**
 * @defgroup collections Collections
 * @{
 */

/**
* @defgroup ring_buffer Ring Buffer
* @{
*/


/**
* @brief Fixed-size message ring buffer.
* Stores a fixed number of messages of identical size using
* application-provided backing storage.
*/
typedef struct
{
  uint8_t* buffer; ///< Backing storage for messages (application-owned).
  size_t message_size; ///< Size in bytes of each message.
  size_t capacity; ///< Maximum number of messages that can fit.
  size_t head; ///< Write position (next free slot).
  size_t tail; ///< Read position (oldest message).
  size_t count; ///< Number of messages currently in the buffer.
} ring_buffer_t;


/**
* @brief Statically initializes a ring buffer.
* @param storage Backing storage for the messages.
* @param msg_size Size in bytes of each message.
* @param cap Maximum number of messages that can be stored.
* @note The storage must provide at least @p msg_size * @p cap bytes.
*/
#define RING_BUFFER_INITIALIZER(storage, msg_size, cap) \
  ((ring_buffer_t){ \
    .buffer = (storage), \
    .message_size = (msg_size), \
    .capacity = (cap), \
    .head = 0, \
    .tail = 0, \
    .count = 0 \
  })

/**
 * @brief Adds a message to the ring buffer.
 *
 * The message is copied into the next available slot.
 *
 * @param ring_buffer Ring buffer to modify.
 * @param message Message to copy into the buffer.
 *
 * @return true if the message was added, otherwise false if the buffer is full.
 */
bool ring_buffer_push(ring_buffer_t* ring_buffer, void const* message);


/**
 * @brief Removes the oldest message from the ring buffer.
 *
 * The message is copied into the provided buffer.
 *
 * @param ring_buffer Ring buffer to modify.
 * @param message Buffer to copy the message into.
 *
 * @return true if a message was removed, otherwise false if the buffer is empty.
 */
bool ring_buffer_pop(ring_buffer_t* ring_buffer, void* message);


/**
 * @brief Peeks at the oldest message in the ring buffer without removing it.
 *
 * The message is copied into the provided buffer.
 *
 * @param ring_buffer Ring buffer to inspect.
 * @param message Buffer to copy the message into.
 *
 * @return true if a message was copied, otherwise false if the buffer is empty.
 */
bool ring_buffer_peek(ring_buffer_t const* ring_buffer, void* message);


/**
 * @brief Checks if the ring buffer is empty.
 *
 * @param ring_buffer Ring buffer to inspect.
 *
 * @return true if the buffer is empty, otherwise false.
 */
bool ring_buffer_is_empty(ring_buffer_t const* ring_buffer);


/**
 * @brief Checks if the ring buffer is full.
 *
 * @param ring_buffer Ring buffer to inspect.
 *
 * @return true if the buffer is full, otherwise false.
 */
bool ring_buffer_is_full(ring_buffer_t const* ring_buffer);


/**
 * @brief Gets the number of messages currently in the ring buffer.
 *
 * @param ring_buffer Ring buffer to inspect.
 *
 * @return Number of messages in the buffer.
 */
size_t ring_buffer_count(ring_buffer_t const* ring_buffer);


/**
 * @brief Clears all messages from the ring buffer.
 *
 * @param ring_buffer Ring buffer to modify.
 */
void ring_buffer_clear(ring_buffer_t* ring_buffer);

/** @} */ /* end of ring_buffer */
/** @} */ /* end of collections */
/** @} */ /* end of util */

#ifdef __cplusplus
}
#endif
