#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include "io/gpio.h"
#include "util/helpers.h"

// --- SIO / PADS_BANK0 / IO_BANK0 peripheral registers (from RP2040 SVD) ---
#ifndef SIO_BASE
#define SIO_BASE 0XD0000000
#endif

#define SIO_GPIO_IN_OFFSET 0X0004
#define SIO_GPIO_OUT_SET_OFFSET 0X0014
#define SIO_GPIO_OUT_CLR_OFFSET 0X0018
#define SIO_GPIO_OUT_XOR_OFFSET 0X001C
#define SIO_GPIO_OE_SET_OFFSET 0X0024
#define SIO_GPIO_OE_CLR_OFFSET 0X0028

#define PADS_BANK0_BASE 0X4001C000
#define PADS_BANK0_GPIO0_OFFSET 0X0004
#define PADS_BANK0_GPIO0_PDE_MASK 0X4
#define PADS_BANK0_GPIO0_PDE_OFFSET 2
#define PADS_BANK0_GPIO0_PUE_MASK 0X8
#define PADS_BANK0_GPIO0_PUE_OFFSET 3

#define IO_BANK0_BASE 0X40014000
#define IO_BANK0_GPIO0_CTRL_OFFSET 0X0004
#define IO_BANK0_GPIO0_CTRL_FUNCSEL_MASK 0X1F
#define IO_BANK0_GPIO0_CTRL_FUNCSEL_OFFSET 0

#define SIO_GPIO_OE_CLR     ((volatile uint32_t*)(SIO_BASE + SIO_GPIO_OE_CLR_OFFSET))
#define SIO_GPIO_OE_SET     ((volatile uint32_t*)(SIO_BASE + SIO_GPIO_OE_SET_OFFSET))
#define PADS_BANK0_GPIO0    ((volatile uint32_t*)(PADS_BANK0_BASE + PADS_BANK0_GPIO0_OFFSET))
#define IO_BANK0_GPIO0_CTRL ((volatile uint32_t*)(IO_BANK0_BASE + IO_BANK0_GPIO0_CTRL_OFFSET))

typedef enum
{
  GPIO_FUNC_1 = 1U,
  GPIO_FUNC_2 = 2U,
  GPIO_FUNC_3 = 3U,
  GPIO_FUNC_4 = 4U,
  GPIO_FUNC_5 = 5U,
  GPIO_FUNC_6 = 6U,
  GPIO_FUNC_7 = 7U,
  GPIO_FUNC_8 = 8U,
  GPIO_FUNC_9 = 9U
} gpio_func_enum;

static inline void gpio_set_direction(uint32_t const pin, gpio_dir_t const dir)
{
  if (dir == GPIO_DIR_INPUT)
  {
    *SIO_GPIO_OE_CLR = 1U << pin;
  }
  else
  {
    *SIO_GPIO_OE_SET = 1U << pin;
  }
}

static inline void gpio_set_resistor(uint32_t const pin, gpio_pull_t const pull)
{
  volatile uint32_t* pads_bank0_gpio = PADS_BANK0_GPIO0 + pin;
  switch (pull)
  {
  case GPIO_PULL_UP:
    REG_SET_FIELD(pads_bank0_gpio, PADS_BANK0_GPIO0_PUE, 1U);
    REG_SET_FIELD(pads_bank0_gpio, PADS_BANK0_GPIO0_PDE, 0U);
    break;
  case GPIO_PULL_DOWN:
    REG_SET_FIELD(pads_bank0_gpio, PADS_BANK0_GPIO0_PDE, 1U);
    REG_SET_FIELD(pads_bank0_gpio, PADS_BANK0_GPIO0_PUE, 0U);
    break;
  default:
    REG_SET_FIELD(pads_bank0_gpio, PADS_BANK0_GPIO0_PDE, 0U);
    REG_SET_FIELD(pads_bank0_gpio, PADS_BANK0_GPIO0_PUE, 0U);
    break;
  }
}

static inline void gpio_set_func(uint32_t const pin, gpio_func_enum const gpio_func)
{
  volatile uint32_t* io_bank0_gpio = (IO_BANK0_GPIO0_CTRL + (pin * 2));
  REG_SET_FIELD(io_bank0_gpio, IO_BANK0_GPIO0_CTRL_FUNCSEL, gpio_func);
}

#ifdef __cplusplus
}
#endif
