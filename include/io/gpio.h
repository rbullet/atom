#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

/**
 * @defgroup io IO
 * @{
 */

/**
 * @defgroup gpio GPIO
 * @brief General-Purpose Input/Output API.
 *
 * Provides a simple, portable interface for configuring and controlling GPIO
 * pins. Supports pin direction, pull-up/pull-down configuration,
 * reading/writing pin levels and toggling outputs.
 *
 * Unless otherwise specified, all GPIO operations are thread-safe and may be
 * called from any execution context.
 *
 * @ingroup io
 * @{
 */

/**
 * @brief GPIO pin direction.
 */
typedef enum
{
  GPIO_DIR_INPUT = 0,  ///< Pin configured as input.
  GPIO_DIR_OUTPUT = 1  ///< Pin configured as output.
} gpio_dir_t;

/**
 * @brief GPIO pull-resistor configuration.
 */
typedef enum
{
  GPIO_PULL_NONE,  ///< No pull resistor.
  GPIO_PULL_UP,    ///< Pull-up resistor enabled.
  GPIO_PULL_DOWN   ///< Pull-down resistor enabled.
} gpio_pull_t;

/**
 * @brief GPIO logic level.
 */
typedef enum
{
  GPIO_LEVEL_LOW = 0,   ///< Logic low.
  GPIO_LEVEL_HIGH = 1   ///< Logic high.
} gpio_level_t;

/**
 * @brief Initialize the GPIO subsystem.
 *
 * Resets the IO and pad control banks used by GPIO pins.
 *
 * @note Already invoked once during board/system initialization, before
 *       `main()` runs. Application code should not normally call this
 *       function itself.
 *
 * @warning Calling this again after startup resets IO_BANK0/PADS_BANK0 and
 *          will discard any pin configuration performed since boot,
 *          including peripheral pin muxing (e.g. UART TX/RX pins).
 */
void gpio_init(void);

/**
 * @brief Configure a GPIO pin.
 *
 * Sets the direction and pull-resistor configuration of a GPIO pin.
 *
 * @param pin GPIO pin number.
 * @param dir Pin direction.
 * @param pull Pull-resistor configuration.
 *
 * @pre pin must be a valid GPIO pin number for the platform.
 */
void gpio_config(uint32_t pin, gpio_dir_t dir, gpio_pull_t pull);

/**
 * @brief Read the logic level of a GPIO pin.
 *
 * @param pin GPIO pin number.
 * @return Current logic level.
 *
 * @pre pin must be a valid GPIO pin number for the platform.
 */
gpio_level_t gpio_read(uint32_t pin);

/**
 * @brief Set the output level of a GPIO pin.
 *
 * @param pin GPIO pin number.
 * @param level Logic level to set.
 *
 * @pre pin must be configured as GPIO_DIR_OUTPUT.
 */
void gpio_write(uint32_t pin, gpio_level_t level);

/**
 * @brief Toggle the output level of a GPIO pin.
 *
 * @param pin GPIO pin number.
 *
 * @pre pin must be configured as GPIO_DIR_OUTPUT.
 */
void gpio_toggle(uint32_t pin);

/** @} */ /* end of gpio */
/** @} */ /* end of io */

#ifdef __cplusplus
}
#endif