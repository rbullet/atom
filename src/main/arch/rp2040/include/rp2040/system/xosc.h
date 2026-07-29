#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <stdbool.h>
#include <stdint.h>
#include "rp2040/rp2040.h"
#include "util/helpers.h"

#define XOSC_STATUS     ((volatile uint32_t*)(XOSC_BASE + XOSC_STATUS_OFFSET))
#define XOSC_CTRL       ((volatile uint32_t*)(XOSC_BASE + XOSC_CTRL_OFFSET))
#define XOSC_STARTUP    ((volatile uint32_t*)(XOSC_BASE + XOSC_STARTUP_OFFSET))
#define XOSC_DORMANT    ((volatile uint32_t*)(XOSC_BASE + XOSC_DORMANT_OFFSET))

// --- Frequency range selection ---
typedef enum
{
  XOSC_CTRL_FREQ_1_15MHZ = 0xAA0
} xosc_ctrl_freq_range_enum;

// --- Enable/disable commands ---
typedef enum
{
  XOSC_CTRL_DISABLE = 0xD1E,
  XOSC_CTRL_ENABLE = 0xFAB
} xosc_ctrl_enable_enum;

// --- Startup delay defaults ---
typedef enum
{
  XOSC_STARTUP_DEFAULT_DELAY = 0x00C4
} xosc_startup_delay_enum;

// --- Dormant/wake states ---
typedef enum
{
  XOSC_DORMANT_DORMANT = 0X636F6D61,
  XOSC_DORMANT_WAKE = 0X77616B65
} xosc_dormant_enum;

// --- Clear bad write bit in XOSC ---
static inline void xosc_clear_badwrite_bit(void)
{
  REG_SET_FIELD(*XOSC_STATUS, XOSC_STATUS_BADWRITE, 1);
}

// --- Get crystal oscillator frequency range ---
static inline xosc_ctrl_freq_range_enum xosc_get_freq_range()
{
  return REG_GET_FIELD(*XOSC_CTRL, XOSC_CTRL_FREQ_RANGE);
}

// --- Set crystal oscillator frequency range ---
static inline void xosc_set_freq_range(xosc_ctrl_freq_range_enum const freq)
{
  REG_SET_FIELD(*XOSC_CTRL, XOSC_CTRL_FREQ_RANGE, freq);
}

// --- Check if XOSC is stable ---
static inline bool xosc_is_stable(void)
{
  return REG_GET_FIELD(*XOSC_STATUS, XOSC_STATUS_STABLE);
}

// --- Enable or disable XOSC ---
static inline void xosc_set_enable(bool const enable)
{
  REG_SET_FIELD(*XOSC_CTRL, XOSC_CTRL_ENABLE, (enable ? XOSC_CTRL_ENABLE : XOSC_CTRL_DISABLE));
}

// --- Get startup delay for XOSC ---
static inline xosc_startup_delay_enum xosc_get_startup_delay()
{
  return REG_GET_FIELD(*XOSC_STARTUP, XOSC_STARTUP_DELAY);
}

// --- Set startup delay for XOSC ---
static inline void xosc_set_startup_delay(xosc_startup_delay_enum delay)
{
  REG_SET_FIELD(*XOSC_STARTUP, XOSC_STARTUP_DELAY, delay);
}

// --- Get dormant/wake state ---
static inline xosc_dormant_enum xosc_get_dormant()
{
  return *XOSC_DORMANT;
}

// --- Set dormant/wake state ---
static inline void xosc_set_dormant(xosc_dormant_enum const dormant)
{
  *XOSC_DORMANT = dormant;
}

#ifdef __cplusplus
}
#endif
