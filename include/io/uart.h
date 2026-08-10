#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stddef.h>
#include <stdint.h>

/**
 * @defgroup io IO
 * @{
 */

/**
 * @defgroup uart UART
 * @brief UART (Universal Asynchronous Receiver/Transmitter) API.
 *
 * Provides a simple, portable interface for UART peripherals.
 * Supports initialization, enable/disable, and blocking read/write operations.
 *
 * @ingroup io
 * @{
 */

/**
 * @brief Opaque UART type, board-specific implementation hidden.
 */
typedef volatile struct uart_t uart_t;

/**
 * @{
 * @name Predefined UART instances
 * Standard UART instances provided by the platform.
 */

/**
 * @brief UART0 instance.
 *
 * Available on all supported platforms. Always a valid pointer.
 */
extern uart_t* const uart0;

/**
 * @brief UART1 instance.
 *
 * May be NULL on some architectures. Always check before use on portable code.
 */
extern uart_t* const uart1;

/**
 * @}
 */

/**
 * @brief Initialize a UART with a given baud rate.
 *
 * Configures the UART for 8N1 communication and enables TX/RX.
 *
 * @note The UART instance pointer must be valid. On some boards, `uart1` may be
 * NULL.
 *
 * @param uart UART instance.
 * @param baud_rate Baud rate in bits per second.
 *
 * @pre uart must point to a valid UART instance (uart0, uart1, etc.).
 * @pre baud_rate must be a standard baud rate (e.g. 9600, 115200).
 *
 * @post UART is initialized and ready for read/write operations.
 *
 * @warning Must be called exactly once per UART before any read/write
 * operations.
 */
void uart_init(uart_t* uart, uint32_t baud_rate);

/**
 * @brief Check whether a UART is currently enabled.
 *
 * @param uart UART instance.
 * @return true if the UART is enabled, false otherwise.
 *
 * @note Safe to call from any execution context.
 */
bool uart_is_enabled(uart_t const* uart);

/**
 * @brief Enable a UART peripheral.
 *
 * @param uart UART instance.
 *
 * @pre uart must be a valid UART pointer (typically after uart_init()).
 *
 * @post UART peripheral is enabled and ready for I/O.
 *
 * @note Thread-safe.
 */
void uart_enable(uart_t* uart);

/**
 * @brief Disable a UART peripheral.
 *
 * @param uart UART instance.
 *
 * @pre uart must be a valid UART pointer.
 *
 * @post UART peripheral is disabled. RX/TX operations will not function.
 *
 * @note Thread-safe.
 */
void uart_disable(uart_t* uart);

/**
 * @brief Read bytes from a UART.
 *
 * Blocks until `len` bytes have been received.
 *
 * @param uart UART instance.
 * @param buffer Pointer to the destination buffer.
 * @param len Number of bytes to read.
 *
 * @return Number of bytes transferred.
 *
 * @pre uart must be a valid initialized UART pointer.
 * @pre buffer must point to at least `len` bytes of writable memory.
 * @pre uart must be enabled.
 *
 * @note Blocks the calling context until all requested bytes are received.
 */
size_t uart_read(uart_t* uart, uint8_t* buffer, size_t len);

/**
 * @brief Write bytes to a UART.
 *
 * Blocks until all bytes have been transmitted.
 *
 * @param uart UART instance.
 * @param buffer Pointer to the source buffer.
 * @param len Number of bytes to write.
 *
 * @return Number of bytes transferred.
 *
 * @pre uart must be a valid initialized UART pointer.
 * @pre buffer must point to at least `len` bytes of readable memory.
 * @pre uart must be enabled.
 *
 * @note Blocks the calling context until all requested bytes are transmitted.
 */
size_t uart_write(uart_t* uart, uint8_t const* buffer, size_t len);

/**
 * @brief Read a single byte from a UART.
 *
 * @param uart UART instance.
 * @return Received byte.
 *
 * @note Blocks the calling context until one byte is received.
 */
static inline uint8_t uart_read_byte(uart_t* uart)
{
  uint8_t byte;
  uart_read(uart, &byte, 1);
  return byte;
}

/**
 * @brief Write a single byte to a UART.
 *
 * @param uart UART instance.
 * @param byte Byte to transmit.
 *
 * @note Blocks the calling context until the byte has been transmitted.
 */
static inline void uart_write_byte(uart_t* uart, uint8_t const byte)
{
  uart_write(uart, &byte, 1);
}

/** @} */ /* end of uart group */
/** @} */ /* end of io group */

#ifdef __cplusplus
}
#endif