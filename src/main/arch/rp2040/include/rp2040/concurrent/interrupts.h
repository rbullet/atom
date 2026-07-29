#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include "concurrent/interrupts.h"

// --- IRQ enumeration ---
typedef enum
{
  // System exceptions that can be overridden
  IRQ_SVC = 11,
  IRQ_PEND_SV = 14,
  IRQ_SYS_TICK = 15,

  // Peripheral IRQs
  IRQ_TIMER_0 = 16,
  IRQ_TIMER_1,
  IRQ_TIMER_2,
  IRQ_TIMER_3,
  IRQ_PWM,
  IRQ_USBCTRL,
  IRQ_XIP,
  IRQ_PIO0_0,
  IRQ_PIO0_1,
  IRQ_PIO1_0,
  IRQ_PIO1_1,
  IRQ_DMA_0,
  IRQ_DMA_1,
  IRQ_IO_BANK_0,
  IRQ_IO_QSPI,
  IRQ_SIO_PROC0,
  IRQ_SIO_PROC1,
  IRQ_CLOCKS,
  IRQ_SPI0,
  IRQ_SPI1,
  IRQ_UART0,
  IRQ_UART1,
  IRQ_ADC_FIFO,
  IRQ_I2C0,
  IRQ_I2C1,
  IRQ_RTC
} interrupt_enum;

// --- IRQ handler function pointer type ---
typedef void (*irq_handler_func_t)(void);

// --- Initialize interrupt system ---
void interrupts_init(void);

// --- Get current IRQ handler ---
irq_handler_func_t interrupts_get_handler(interrupt_enum irq);

// --- Set new IRQ handler, return previous ---
irq_handler_func_t interrupts_set_handler(interrupt_enum irq, irq_handler_func_t handler);

#ifdef __cplusplus
}
#endif
