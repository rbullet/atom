#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include "io/gpio.h"
#include "rp2040/rp2040.h"
#include "util/helpers.h"

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
    REG_SET_FIELD(*pads_bank0_gpio, PADS_BANK0_GPIO0_PUE, 1U);
    REG_SET_FIELD(*pads_bank0_gpio, PADS_BANK0_GPIO0_PDE, 0U);
    break;
  case GPIO_PULL_DOWN:
    REG_SET_FIELD(*pads_bank0_gpio, PADS_BANK0_GPIO0_PDE, 1U);
    REG_SET_FIELD(*pads_bank0_gpio, PADS_BANK0_GPIO0_PUE, 0U);
    break;
  default:
    REG_SET_FIELD(*pads_bank0_gpio, PADS_BANK0_GPIO0_PDE, 0U);
    REG_SET_FIELD(*pads_bank0_gpio, PADS_BANK0_GPIO0_PUE, 0U);
    break;
  }
}

static inline void gpio_set_func(uint32_t const pin, gpio_func_enum const gpio_func)
{
  volatile uint32_t* io_bank0_gpio = (IO_BANK0_GPIO0_CTRL + (pin * 2));
  REG_SET_FIELD(*io_bank0_gpio, IO_BANK0_GPIO0_CTRL_FUNCSEL, gpio_func);
}

#ifdef __cplusplus
}
#endif
