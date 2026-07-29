#include <stdint.h>
#include <string.h>
#include "rp2040/concurrent/interrupts.h"

#define IRQ_VECTOR_COUNT 48

// --- System IRQs ---

// @formatter:off
// --- Forward declarations for IRQ handlers ---
void irq_handler_reset(void)         __attribute__((naked));                        // Reset handler
void irq_handler_nmi(void)           __attribute__((alias("irq_handler_default"))); // NMI handler
void irq_handler_hard_fault(void)    __attribute__((alias("irq_handler_default"))); // Hard fault handler
void irq_handler_mem_manage(void)    __attribute__((alias("irq_handler_default"))); // Memory management fault
void irq_handler_bus_fault(void)     __attribute__((alias("irq_handler_default"))); // Bus fault
void irq_handler_usage_fault(void)   __attribute__((alias("irq_handler_default"))); // Usage fault
void irq_handler_secure_fault(void)  __attribute__((alias("irq_handler_default"))); // Secure fault
void irq_handler_svc(void)           __attribute__((alias("irq_handler_default"))); // SVC handler
void irq_handler_debug_mon(void)     __attribute__((alias("irq_handler_default"))); // Debug monitor
void irq_handler_pend_sv(void)       __attribute__((alias("irq_handler_default"))); // PendSV handler
void irq_handler_sys_tick(void)      __attribute__((alias("irq_handler_default"))); // SysTick handler

void irq_handler_timer_0(void)       __attribute__((alias("irq_handler_default"))); // Timer 0
void irq_handler_timer_1(void)       __attribute__((alias("irq_handler_default"))); // Timer 1
void irq_handler_timer_2(void)       __attribute__((alias("irq_handler_default"))); // Timer 2
void irq_handler_timer_3(void)       __attribute__((alias("irq_handler_default"))); // Timer 3
void irq_handler_pwm(void)           __attribute__((alias("irq_handler_default"))); // PWM
void irq_handler_usbctrl(void)       __attribute__((alias("irq_handler_default"))); // USB controller
void irq_handler_xip(void)           __attribute__((alias("irq_handler_default"))); // XIP
void irq_handler_pio0_0(void)        __attribute__((alias("irq_handler_default"))); // PIO0 IRQ 0
void irq_handler_pio0_1(void)        __attribute__((alias("irq_handler_default"))); // PIO0 IRQ 1
void irq_handler_pio1_0(void)        __attribute__((alias("irq_handler_default"))); // PIO1 IRQ 0
void irq_handler_pio1_1(void)        __attribute__((alias("irq_handler_default"))); // PIO1 IRQ 1
void irq_handler_dma_0(void)         __attribute__((alias("irq_handler_default"))); // DMA channel 0
void irq_handler_dma_1(void)         __attribute__((alias("irq_handler_default"))); // DMA channel 1
void irq_handler_io_bank_0(void)     __attribute__((alias("irq_handler_default"))); // IO Bank 0
void irq_handler_io_qspi(void)       __attribute__((alias("irq_handler_default"))); // IO QSPI
void irq_handler_sio_proc0(void)     __attribute__((alias("irq_handler_default"))); // SIO processor 0
void irq_handler_sio_proc1(void)     __attribute__((alias("irq_handler_default"))); // SIO processor 1
void irq_handler_clocks(void)        __attribute__((alias("irq_handler_default"))); // Clock handler
void irq_handler_spi0(void)          __attribute__((alias("irq_handler_default"))); // SPI 0
void irq_handler_spi1(void)          __attribute__((alias("irq_handler_default"))); // SPI 1
void irq_handler_uart0(void)         __attribute__((alias("irq_handler_default"))); // UART 0
void irq_handler_uart1(void)         __attribute__((alias("irq_handler_default"))); // UART 1
void irq_handler_adc_fifo(void)      __attribute__((alias("irq_handler_default"))); // ADC FIFO
void irq_handler_i2c0(void)          __attribute__((alias("irq_handler_default"))); // I2C 0
void irq_handler_i2c1(void)          __attribute__((alias("irq_handler_default"))); // I2C 1
void irq_handler_rtc(void)           __attribute__((alias("irq_handler_default"))); // RTC

__attribute__((used)) void irq_handler_default(void); // Default handler (infinite wait)

extern uint32_t const _emsp0; // End of MSP for core 0

// --- Relocated vector table in RAM for dynamic overrides ---
__attribute__((used, aligned(256))) volatile uint32_t const vector_table[IRQ_VECTOR_COUNT] __attribute__((section(".vector_table"))) = {
  (uint32_t)&_emsp0,                  // Initial stack pointer
  (uint32_t)irq_handler_reset,       // Reset handler
  (uint32_t)irq_handler_nmi,         // NMI handler
  (uint32_t)irq_handler_hard_fault,  // Hard fault handler
  (uint32_t)irq_handler_mem_manage,  // Memory management fault
  (uint32_t)irq_handler_bus_fault,   // Bus fault
  (uint32_t)irq_handler_usage_fault, // Usage fault
  (uint32_t)irq_handler_secure_fault,// Secure fault
  0,                                  // Reserved
  0,                                  // Reserved
  0,                                  // Reserved
  (uint32_t)irq_handler_svc,          // SVC handler
  (uint32_t)irq_handler_debug_mon,    // Debug monitor
  0,                                  // Reserved
  (uint32_t)irq_handler_pend_sv,      // PendSV handler
  (uint32_t)irq_handler_sys_tick,     // SysTick handler
  (uint32_t)irq_handler_timer_0,      // Timer 0
  (uint32_t)irq_handler_timer_1,      // Timer 1
  (uint32_t)irq_handler_timer_2,      // Timer 2
  (uint32_t)irq_handler_timer_3,      // Timer 3
  (uint32_t)irq_handler_pwm,          // PWM
  (uint32_t)irq_handler_usbctrl,      // USB controller
  (uint32_t)irq_handler_xip,          // XIP
  (uint32_t)irq_handler_pio0_0,       // PIO0 IRQ 0
  (uint32_t)irq_handler_pio0_1,       // PIO0 IRQ 1
  (uint32_t)irq_handler_pio1_0,       // PIO1 IRQ 0
  (uint32_t)irq_handler_pio1_1,       // PIO1 IRQ 1
  (uint32_t)irq_handler_dma_0,        // DMA channel 0
  (uint32_t)irq_handler_dma_1,        // DMA channel 1
  (uint32_t)irq_handler_io_bank_0,    // IO Bank 0
  (uint32_t)irq_handler_io_qspi,      // IO QSPI
  (uint32_t)irq_handler_sio_proc0,    // SIO processor 0
  (uint32_t)irq_handler_sio_proc1,    // SIO processor 1
  (uint32_t)irq_handler_clocks,       // Clock handler
  (uint32_t)irq_handler_spi0,         // SPI 0
  (uint32_t)irq_handler_spi1,         // SPI 1
  (uint32_t)irq_handler_uart0,        // UART 0
  (uint32_t)irq_handler_uart1,        // UART 1
  (uint32_t)irq_handler_adc_fifo,     // ADC FIFO
  (uint32_t)irq_handler_i2c0,         // I2C 0
  (uint32_t)irq_handler_i2c1,         // I2C 1
  (uint32_t)irq_handler_rtc           // RTC
};

// --- Interrupt vector table (placed in .vector_table section) ---
__attribute__((section(".vector_table_ram"), aligned(256))) uint32_t relocated_vector_table[IRQ_VECTOR_COUNT];
// @formatter:on

void interrupts_init(void)
{
  memcpy(&relocated_vector_table, (void const*)&vector_table, sizeof(vector_table));
  WITH_INTERRUPTS_DISABLED
  {
    volatile uint32_t* vtor = (volatile uint32_t*)0xE000ED08; // VTOR register
    *vtor = ((uint32_t)&relocated_vector_table) & ~0xFFU;
    __asm__ volatile("dsb");
    __asm__ volatile("isb");
  }
}

// --- Default IRQ handler (infinite wait loop) ---
__attribute__((used)) void irq_handler_default(void)
{
  while (1);
}

// --- Read current handler from relocated table ---
irq_handler_func_t interrupts_get_handler(interrupt_enum const irq)
{
  return (irq_handler_func_t)relocated_vector_table[irq];
}

// --- Set new handler in relocated table, return old handler ---
irq_handler_func_t interrupts_set_handler(interrupt_enum const irq, irq_handler_func_t const handler)
{
  irq_handler_func_t const old_handler = interrupts_get_handler(irq); // save old handler
  relocated_vector_table[irq] = ((uint32_t)handler); // write new handler
  return old_handler; // return previous handler
}

bool interrupts_are_enabled(void)
{
  uint32_t primask;
  __asm volatile ("mrs %0, primask" : "=r" (primask));
  return (primask == 0);
}

//--- Enable interrupts and return previous PRIMASK state ---
uint32_t interrupts_enable(void)
{
  uint32_t primask;
  __asm__ volatile("mrs %0, primask; cpsie i" : "=r"(primask) : : "memory");
  return primask;
}

//--- Disable interrupts and return previous PRIMASK state ---
uint32_t interrupts_disable(void)
{
  uint32_t primask;
  __asm__ volatile("mrs %0, primask; cpsid i" : "=r"(primask) : : "memory");
  return primask;
}

//--- Restore interrupt state from saved PRIMASK value ---
void interrupts_restore(uint32_t const state)
{
  __asm volatile("msr primask, %0" :: "r"(state) : "memory");
}
