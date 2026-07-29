#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "rp2040/config.h"
#include "util/helpers.h"

#define PLL_SYS_BASE 0X40028000
#define PLL_USB_BASE 0X4002C000

#define PLL_SYS_CS_OFFSET 0X0000
#define PLL_SYS_FBDIV_INT_OFFSET 0X0008
#define PLL_SYS_PRIM_OFFSET 0X000C
#define PLL_SYS_PWR_OFFSET 0X0004


typedef volatile struct pll_t pll_t;

pll_t* const pll_sys =  ((pll_t*)PLL_SYS_BASE);
pll_t* const pll_usb =  ((pll_t*)PLL_USB_BASE);


#define PLL_SYS ((pll_t*)PLL_SYS_BASE)
#define PLL_USB ((pll_t*)PLL_USB_BASE)

#define PLL_CS(pll)        REG((pll) ,PLL_SYS_CS_OFFSET)
#define PLL_FBDIV_INT(pll) REG((pll) ,PLL_SYS_FBDIV_INT_OFFSET)
#define PLL_PRIM(pll)      REG((pll) ,PLL_SYS_PRIM_OFFSET)
#define PLL_PWR(pll)       REG((pll) ,PLL_SYS_PWR_OFFSET)

#define PLL_PWR_MASK 0x3FU

// --- PLL configuration structure ---
typedef struct
{
  uint16_t fbdiv; // Feedback divider
  uint8_t post_div1; // First post divider
  uint8_t post_div2; // Second post divider
  uint8_t refdiv; // Reference divider
  uint32_t achieved_freq; // Frequency actually achieved
} pll_settings_t;

// --- PLL power mode bits ---
typedef enum
{
  PLL_PWR_PD = 1,
  PLL_PWR_DSMPD = 1 << 2,
  PLL_PWR_POSTDIVPD = 1 << 3,
  PLL_PWR_VCOPD = 1 << 5
} pll_pwr_mode_enum;

// --- Get PLL feedback divider ---
static inline uint32_t pll_get_fbdiv_int(pll_t const* pll)
{
  return REG_GET_FIELD(PLL_FBDIV_INT(pll), PLL_SYS_FBDIV_INT_FBDIV_INT);
}

// --- Set PLL feedback divider ---
static inline void pll_set_fbdiv_int(pll_t* pll, uint32_t const fbdiv)
{
  REG_SET_FIELD(PLL_FBDIV_INT(pll), PLL_SYS_FBDIV_INT_FBDIV_INT, fbdiv);
}

// --- Get PLL reference divider ---
static inline uint32_t pll_get_refdiv(pll_t const* pll)
{
  return REG_GET_FIELD(PLL_CS(pll), PLL_SYS_CS_REFDIV);
}

// --- Set PLL reference divider ---
static inline void pll_set_refdiv(pll_t* pll, uint32_t const refdiv)
{
  REG_SET_FIELD(PLL_CS(pll), PLL_SYS_CS_REFDIV, refdiv);
}

// --- Check if PLL is locked ---
static inline bool pll_is_locked(pll_t const* pll)
{
  return REG_GET_FIELD(PLL_CS(pll), PLL_SYS_CS_LOCK);
}

// --- Get PLL post divider 1 ---
static inline uint32_t pll_get_postdiv1(pll_t const* pll)
{
  return REG_GET_FIELD(PLL_PRIM(pll), PLL_SYS_PRIM_POSTDIV1);
}

// --- Set PLL post divider 1 ---
static inline void pll_set_postdiv1(pll_t* pll, uint32_t postdiv1)
{
  REG_SET_FIELD(PLL_PRIM(pll), PLL_SYS_PRIM_POSTDIV1, postdiv1);
}

// --- Get PLL post divider 2 ---
static inline uint32_t pll_get_postdiv2(pll_t* pll)
{
  return REG_GET_FIELD(PLL_PRIM(pll), PLL_SYS_PRIM_POSTDIV2);
}

// --- Set PLL post divider 2 ---
static inline void pll_set_postdiv2(pll_t* pll, uint32_t postdiv2)
{
  REG_SET_FIELD(PLL_PRIM(pll), PLL_SYS_PRIM_POSTDIV2, postdiv2);
}

// --- Disable specific PLL power mode(s) ---
static inline void pll_disable_pwr_mode(pll_t* pll, uint32_t pll_pwr_mode)
{
  PLL_PWR(pll) |= (pll_pwr_mode & PLL_PWR_MASK);
}

// --- Enable specific PLL power mode(s) ---
static inline void pll_enable_pwr_mode(pll_t* pll, uint32_t pll_pwr_mode)
{
  PLL_PWR(pll) &= ~(pll_pwr_mode & PLL_PWR_MASK);
}

// --- Compute PLL settings for target frequency ---
static inline pll_settings_t pll_compute_settings(uint32_t const f_target)
{
  pll_settings_t best = {.fbdiv = 0, .post_div1 = 0, .post_div2 = 0, .refdiv = 0, .achieved_freq = 0};
  uint32_t best_error = 0xFFFFFFFF;

  const uint8_t refdiv_min = 1;
  const uint8_t refdiv_max = 63; // RP2040 max refdiv

  for (uint8_t refdiv = refdiv_min; refdiv <= refdiv_max; ++refdiv)
  {
    const uint8_t post_div_min = 1;
    const uint8_t post_div_max = 7;

    for (uint8_t pd1 = post_div_min; pd1 <= post_div_max; ++pd1)
    {
      for (uint8_t pd2 = post_div_min; pd2 <= post_div_max; ++pd2)
      {
        const uint16_t fbdiv_min = 16;
        const uint16_t fbdiv_max = 320;
        const uint32_t numerator = f_target * pd1 * pd2 * refdiv;
        const uint32_t fbdiv = (numerator + XOSC_FREQUENCY_HZ / 2) / XOSC_FREQUENCY_HZ;

        if (fbdiv < fbdiv_min || fbdiv > fbdiv_max) continue;

        const uint32_t f_actual = (XOSC_FREQUENCY_HZ * fbdiv) / (uint32_t)(refdiv * pd1 * pd2);
        const uint32_t error = (f_actual > f_target) ? (f_actual - f_target) : (f_target - f_actual);

        if (error < best_error)
        {
          best_error = error;
          best.fbdiv = (uint16_t)fbdiv;
          best.post_div1 = pd1;
          best.post_div2 = pd2;
          best.refdiv = refdiv;
          best.achieved_freq = f_actual;

          if (error == 0) return best; // perfect match
        }
      }
    }
  }

  return best;
}

#ifdef __cplusplus
}
#endif
