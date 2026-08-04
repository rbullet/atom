#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "io/uart.h"
#include "util/helpers.h"

// --- UART peripheral registers (from RP2040 SVD) ---
#define UART0_BASE 0X40034000
#define UART1_BASE 0X40038000
#define UART0_UARTDR_OFFSET 0X0000
#define UART0_UARTFR_OFFSET 0X0018
#define UART0_UARTFR_RXFE_MASK 0X10
#define UART0_UARTFR_RXFE_OFFSET 4
#define UART0_UARTFR_TXFF_MASK 0X20
#define UART0_UARTFR_TXFF_OFFSET 5
#define UART0_UARTIBRD_OFFSET 0X0024
#define UART0_UARTIBRD_BAUD_DIVINT_MASK 0XFFFF
#define UART0_UARTIBRD_BAUD_DIVINT_OFFSET 0
#define UART0_UARTFBRD_OFFSET 0X0028
#define UART0_UARTFBRD_BAUD_DIVFRAC_MASK 0X3F
#define UART0_UARTFBRD_BAUD_DIVFRAC_OFFSET 0
#define UART0_UARTLCR_H_OFFSET 0X002C
#define UART0_UARTLCR_H_WLEN_MASK 0X60
#define UART0_UARTLCR_H_WLEN_OFFSET 5
#define UART0_UARTLCR_H_FEN_MASK 0X10
#define UART0_UARTLCR_H_FEN_OFFSET 4
#define UART0_UARTCR_OFFSET 0X0030

#define UART_CR_MASK    0XFF87

// --- UART Word Length Settings ---
typedef enum
{
  UARTLCR_H_WLEN_8_BITS = 3,
  UARTLCR_H_WLEN_7_BITS = 2,
  UARTLCR_H_WLEN_6_BITS = 1,
  UARTLCR_H_WLEN_5_BITS = 0
} uartlcr_h_wlen_enum;

// --- UART FIFO Enable/Disable ---
typedef enum
{
  UARTLCR_H_FEN_FIFO_DISABLED = 0,
  UARTLCR_H_FEN_FIFO_ENABLED = 1,
} uartlcr_h_fen_enum;

// --- UART Control Register Bits ---
typedef enum
{
  UART_UARTCR_UARTEN = 1 << 0,
  UART_UARTCR_SIREN = 1 << 1,
  UART_UARTCR_SIRLP = 1 << 2,
  UART_UARTCR_LBE = 1 << 7,
  UART_UARTCR_TXE = 1 << 8,
  UART_UARTCR_RXE = 1 << 9,
  UART_UARTCR_DTR = 1 << 10,
  UART_UARTCR_RTS = 1 << 11,
  UART_UARTCR_OUT1 = 1 << 12,
  UART_UARTCR_OUT2 = 1 << 13,
  UART_UARTCR_RTSEN = 1 << 14,
  UART_UARTCR_CTSEN = 1 << 15
} uart_uartcr_enum;

// --- UART Baud Rate Parameters ---
typedef struct
{
  uint32_t ibrd; // Integer Baud Rate Divisor
  uint32_t fbrd; // Fractional Baud Rate Divisor
} baud_rate_params_t;

// --- Check if a UART mode is enabled ---
static inline bool uart_has_mode(uart_t const* uart, uint32_t const uart_mode)
{
  return (REG_READ(REG(uart, UART0_UARTCR_OFFSET)) & (uart_mode & UART_CR_MASK)) != 0;
}

// --- Disable a UART mode ---
static inline void uart_disable_mode(uart_t* uart, uint32_t const uart_mode)
{
  REG_WRITE(REG(uart, UART0_UARTCR_OFFSET), REG_READ(REG(uart, UART0_UARTCR_OFFSET)) & ~(uart_mode & UART_CR_MASK));
}

// --- Enable a UART mode ---
static inline void uart_enable_mode(uart_t* uart, uint32_t const uart_mode)
{
  REG_WRITE(REG(uart, UART0_UARTCR_OFFSET), REG_READ(REG(uart, UART0_UARTCR_OFFSET)) | (uart_mode & UART_CR_MASK));
}

// --- Check if RX FIFO is empty ---
static inline bool uart_rx_fifo_is_empty(uart_t const* uart)
{
  return REG_GET_FIELD(REG(uart, UART0_UARTFR_OFFSET), UART0_UARTFR_RXFE) == 1U;
}

// --- Check if TX FIFO is full ---
static inline bool uart_tx_fifo_is_full(uart_t const* uart)
{
  return REG_GET_FIELD(REG(uart, UART0_UARTFR_OFFSET), UART0_UARTFR_TXFF) == 1;
}

// --- Compute baud rate divisors for a given baud rate ---
static inline baud_rate_params_t uart_get_baud_rate_params(uint32_t const baud_rate)
{
  if (baud_rate == 0)
  {
    return (baud_rate_params_t){.ibrd = 0, .fbrd = 0};
  }

  uint32_t const baud16 = 16 * baud_rate;
  uint32_t const ibrd = CPU_FREQUENCY_HZ / baud16;
  uint32_t const fbrd = ((CPU_FREQUENCY_HZ % baud16) * 64 + baud16 / 2) / baud16;

  return (baud_rate_params_t){.ibrd = ibrd, .fbrd = fbrd};
}

#ifdef __cplusplus
}
#endif
