#include "util/collection/ring_buffer.h"

#include <string.h>

static inline size_t ring_buffer_offset(ring_buffer_t const* ring_buffer, size_t const index)
{
  return index * ring_buffer->message_size;
}

bool ring_buffer_push(ring_buffer_t* ring_buffer, void const* message)
{
  if (ring_buffer_is_full(ring_buffer))
  {
    return false;
  }

  size_t const write_offset = ring_buffer_offset(ring_buffer, ring_buffer->head);
  memcpy(&ring_buffer->buffer[write_offset], message, ring_buffer->message_size);

  ring_buffer->head = (ring_buffer->head + 1) % ring_buffer->capacity;
  ring_buffer->count++;

  return true;
}

bool ring_buffer_pop(ring_buffer_t* ring_buffer, void* message)
{
  if (ring_buffer_is_empty(ring_buffer))
  {
    return false;
  }

  size_t const read_offset = ring_buffer_offset(ring_buffer, ring_buffer->tail);
  memcpy(message, &ring_buffer->buffer[read_offset], ring_buffer->message_size);

  ring_buffer->tail = (ring_buffer->tail + 1) % ring_buffer->capacity;
  ring_buffer->count--;

  return true;
}

bool ring_buffer_peek(ring_buffer_t const* ring_buffer, void* message)
{
  if (ring_buffer_is_empty(ring_buffer))
  {
    return false;
  }

  size_t const read_offset = ring_buffer_offset(ring_buffer, ring_buffer->tail);
  memcpy(message, &ring_buffer->buffer[read_offset], ring_buffer->message_size);

  return true;
}

bool ring_buffer_is_empty(ring_buffer_t const* ring_buffer)
{
  return ring_buffer->count == 0;
}

bool ring_buffer_is_full(ring_buffer_t const* ring_buffer)
{
  return ring_buffer->count == ring_buffer->capacity;
}

size_t ring_buffer_count(ring_buffer_t const* ring_buffer)
{
  return ring_buffer->count;
}

void ring_buffer_clear(ring_buffer_t* ring_buffer)
{
  ring_buffer->head = 0;
  ring_buffer->tail = 0;
  ring_buffer->count = 0;
}
