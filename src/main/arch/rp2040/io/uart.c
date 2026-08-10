#include <stdbool.h>

#include "rp2040/atom.h"

#define UART_UARTDR(uart)  REG((uart) ,UART0_UARTDR_OFFSET)

// --- UART instances ---

uart_t* const uart0 = (uart_t*)UART0_BASE;
uart_t* const uart1 = (uart_t*)UART1_BASE;

// --- UART configuration ---

void uart_init(uart_t* uart, uint32_t const baud_rate)
{
  baud_rate_params_t const baud_rate_params = uart_get_baud_rate_params(baud_rate);
  REG_SET_FIELD(REG(uart, UART0_UARTIBRD_OFFSET), UART0_UARTIBRD_BAUD_DIVINT, baud_rate_params.ibrd);
  REG_SET_FIELD(REG(uart, UART0_UARTFBRD_OFFSET), UART0_UARTFBRD_BAUD_DIVFRAC, baud_rate_params.fbrd);
  REG_SET_FIELD(REG(uart, UART0_UARTLCR_H_OFFSET), UART0_UARTLCR_H_WLEN, UARTLCR_H_WLEN_8_BITS);
  REG_SET_FIELD(REG(uart, UART0_UARTLCR_H_OFFSET), UART0_UARTLCR_H_FEN, UARTLCR_H_FEN_FIFO_ENABLED);
  uart_enable_mode(uart, UART_UARTCR_TXE | UART_UARTCR_RXE | UART_UARTCR_UARTEN);
}

// --- UART operations ---

bool uart_is_enabled(uart_t const* uart)
{
  return uart_has_mode(uart, UART_UARTCR_UARTEN);
}

void uart_disable(uart_t* uart)
{
  uart_disable_mode(uart, UART_UARTCR_UARTEN);
}

void uart_enable(uart_t* uart)
{
  uart_enable_mode(uart, UART_UARTCR_UARTEN);
}

// --- UART I/O ---

size_t uart_read(uart_t* uart, uint8_t* buffer, size_t len)
{
  for (uint32_t i = 0; i < len; i++)
  {
    while (uart_rx_fifo_is_empty(uart))
    {
      __asm volatile ("nop");
    }
    buffer[i] = (uint8_t)(REG_READ(UART_UARTDR(uart)));
  }
  return len;
}

size_t uart_write(uart_t* uart, uint8_t const* buffer, size_t len)
{
  for (size_t i = 0; i < len; i++)
  {
    while (uart_tx_fifo_is_full(uart))
    {
      __asm volatile ("nop");
    }
    REG_WRITE(UART_UARTDR(uart), buffer[i] & 0xFF);
  }
  return len;
}


