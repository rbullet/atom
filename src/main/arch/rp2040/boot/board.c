#include <string.h>

#include <atom.h>
#include "rp2040/atom.h"
#include "rp2040/io/gpio.h"
#include "rp2040/concurrent/interrupts.h"
#include "rp2040/concurrent/scheduler.h"
#include "rp2040/concurrent/hardware_spinlock.h"

// --- Forward declarations ---
static void board_init_xosc(void);
static void board_reset_clocks(void);
static void board_init_subsystems(void);
static void board_init_spinlocks(void);
static void board_init_interrupts(void);
static void board_init_pll(void);
static void board_init_clocks(void);
static void board_init_uart(void);
static void board_init_logging(void);
static void board_init_scheduler();

// --- Perform full board initialization ---
void board_init(void)
{
  board_init_spinlocks();
  board_init_interrupts();
  board_reset_clocks();
  board_init_xosc();
  board_init_subsystems();
  board_init_pll();
  board_init_clocks();
  board_init_uart();
  board_init_logging();
  board_init_scheduler();
}

static void board_init_interrupts(void)
{
  interrupts_init();
}

// --- Reset system clocks to safe defaults ---
static void board_reset_clocks(void)
{
  clocks_clk_ref_set_src(CLK_REF_SRC_ROSC_CLKSRC_PH);
  clocks_clk_sys_set_src(CLK_SYS_SRC_CLK_REF);
  clocks_clk_peri_set_enabled(false);
  clocks_clk_peri_set_auxsrc(CLK_PERI_AUXSRC_CLK_SYS);
}

// --- Initialize all spinlocks to unlocked state ---
static void board_init_spinlocks(void)
{
  hardware_spinlock_init();
}

// --- Initialize external crystal oscillator (XOSC) ---
static void board_init_xosc(void)
{
  xosc_clear_badwrite_bit();
  xosc_set_startup_delay(XOSC_STARTUP_DEFAULT_DELAY);
  xosc_set_freq_range(XOSC_CTRL_FREQ_1_15MHZ);
  xosc_set_dormant(XOSC_DORMANT_WAKE);
  xosc_set_enable(true);
  while (!xosc_is_stable())
  {
    __asm volatile("nop");
  }
}

// --- Reset essential subsystems (PLL, IO, UART) ---
static void board_init_subsystems(void)
{
  reset(RESETS_RESET_PLL_SYS | RESETS_RESET_IO_BANK0 | RESETS_RESET_PADS_BANK0 | RESETS_RESET_UART0);
}

// --- Configure PLL for CPU frequency ---
static void board_init_pll(void)
{
  pll_disable_pwr_mode(PLL_SYS, PLL_PWR_PD | PLL_PWR_VCOPD | PLL_PWR_POSTDIVPD);
  pll_settings_t pll_settings = pll_compute_settings(CPU_FREQUENCY_HZ);
  pll_set_refdiv(PLL_SYS, pll_settings.refdiv);
  pll_set_fbdiv_int(PLL_SYS, pll_settings.fbdiv);
  pll_enable_pwr_mode(PLL_SYS, PLL_PWR_PD | PLL_PWR_VCOPD);
  while (!pll_is_locked(PLL_SYS))
  {
    __asm volatile("nop");
  }
  pll_set_postdiv1(PLL_SYS, pll_settings.post_div1);
  pll_set_postdiv2(PLL_SYS, pll_settings.post_div2);
  pll_enable_pwr_mode(PLL_SYS, PLL_PWR_POSTDIVPD);
}

// --- Configure system and peripheral clocks ---
static void board_init_clocks(void)
{
  clocks_clk_ref_set_src(CLK_REF_SRC_XOSC_CLKSRC);
  clocks_clk_sys_set_auxsrc(CLK_SYS_AUXSRC_CLKSRC_PLL_SYS);
  clocks_clk_sys_set_src(CLK_SYS_SRC_CLKSRC_CLK_SYS_AUX);
  clocks_clk_peri_set_enabled(false);
  clocks_clk_peri_set_auxsrc(CLK_PERI_AUXSRC_CLKSRC_PLL_SYS);
  clocks_clk_peri_set_enabled(true);
}

// --- Initialize UART0 for communication ---
static void board_init_uart(void)
{
  if (uart_is_enabled(uart0))
  {
    uart_disable(uart0);
  }
  gpio_set_func(0, GPIO_FUNC_2);
  gpio_set_func(1, GPIO_FUNC_2);
  uart_init(uart0, DEFAULT_UART_BAUD_RATE);
}

// --- Configure logging system output and level ---
static void board_init_logging(void)
{
  log_set_output(stdout);
  log_set_min_level(LOG_LEVEL_INFO);
}

// --- Initialize scheduler and enable interrupts ---
static void board_init_scheduler(void)
{
  scheduler_init();
  interrupts_enable();
}
