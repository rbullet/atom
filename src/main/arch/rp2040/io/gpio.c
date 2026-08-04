#include "rp2040/io/gpio.h"
#include "rp2040/system/resets.h"

#define SIO_GPIO_OUT_SET REG(SIO_BASE, SIO_GPIO_OUT_SET_OFFSET)
#define SIO_GPIO_OUT_CLR REG(SIO_BASE, SIO_GPIO_OUT_CLR_OFFSET)
#define SIO_GPIO_IN      REG(SIO_BASE, SIO_GPIO_IN_OFFSET)
#define SIO_GPIO_OUT_XOR REG(SIO_BASE, SIO_GPIO_OUT_XOR_OFFSET)

void gpio_init(void)
{
  reset(RESETS_RESET_IO_BANK0 | RESETS_RESET_PADS_BANK0);
}

void gpio_config(uint32_t const pin, gpio_dir_t const dir, gpio_pull_t const pull)
{
  gpio_set_resistor(pin, pull);
  gpio_set_func(pin, GPIO_FUNC_5);
  gpio_set_direction(pin, dir);
}

void gpio_write(uint32_t const pin, gpio_level_t const level)
{
  if (level == GPIO_LEVEL_HIGH)
  {
    REG_WRITE(SIO_GPIO_OUT_SET, (1U << pin));
  }
  else
  {
    REG_WRITE(SIO_GPIO_OUT_CLR, (1U << pin));
  }
}

gpio_level_t gpio_read(uint32_t const pin)
{
  return (REG_READ(SIO_GPIO_IN) & (1u << pin)) ? GPIO_LEVEL_HIGH : GPIO_LEVEL_LOW;
}

void gpio_toggle(uint32_t const pin)
{
  REG_WRITE(SIO_GPIO_OUT_XOR, (1U << pin));
}
