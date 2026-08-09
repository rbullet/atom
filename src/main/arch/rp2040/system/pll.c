#include "rp2040/atom.h"

pll_t* const pll_sys = ((pll_t* const)PLL_SYS_BASE);

pll_t* const pll_usb = ((pll_t* const)PLL_USB_BASE);

pll_settings_t pll_compute_settings(uint32_t const f_target)
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