#pragma once
#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>
#include <stdbool.h>
#include "util/helpers.h"

// --- CLOCKS peripheral registers (from RP2040 SVD) ---
#define CLOCKS_BASE 0X40008000
#define CLOCKS_CLK_REF_CTRL_OFFSET 0X0030
#define CLOCKS_CLK_REF_CTRL_SRC_MASK 0X3
#define CLOCKS_CLK_REF_CTRL_SRC_OFFSET 0
#define CLOCKS_CLK_SYS_CTRL_OFFSET 0X003C
#define CLOCKS_CLK_SYS_CTRL_SRC_MASK 0X1
#define CLOCKS_CLK_SYS_CTRL_SRC_OFFSET 0
#define CLOCKS_CLK_SYS_CTRL_AUXSRC_MASK 0XE0
#define CLOCKS_CLK_SYS_CTRL_AUXSRC_OFFSET 5
#define CLOCKS_CLK_PERI_CTRL_OFFSET 0X0048
#define CLOCKS_CLK_PERI_CTRL_ENABLE_MASK 0X800
#define CLOCKS_CLK_PERI_CTRL_ENABLE_OFFSET 11
#define CLOCKS_CLK_PERI_CTRL_AUXSRC_MASK 0XE0
#define CLOCKS_CLK_PERI_CTRL_AUXSRC_OFFSET 5

typedef volatile uint32_t clocks_t;

extern clocks_t* const clocks_clk_ref_ctrl;
extern clocks_t* const clocks_clk_sys_ctrl;
extern clocks_t* const clocks_clk_peri_ctrl;

// --- Reference clock source selection ---
typedef enum
{
  CLK_REF_SRC_ROSC_CLKSRC_PH = 0x0,
  CLK_REF_SRC_CLKSRC_CLK_REF_AUX = 0x1,
  CLK_REF_SRC_XOSC_CLKSRC = 0x2
} clocks_clk_ref_ctrl_src_enum;

// --- System clock source selection ---
typedef enum
{
  CLK_SYS_SRC_CLK_REF = 0x0,
  CLK_SYS_SRC_CLKSRC_CLK_SYS_AUX = 0x1
} clocks_clk_sys_ctrl_src_enum;

// --- System clock auxiliary source selection ---
typedef enum
{
  CLK_SYS_AUXSRC_CLKSRC_PLL_SYS = 0x0,
  CLK_SYS_AUXSRC_CLKSRC_PLL_USB = 0x1,
  CLK_SYS_AUXSRC_ROSC_CLKSRC = 0x2,
  CLK_SYS_AUXSRC_XOSC_CLKSRC = 0x3,
  CLK_SYS_AUXSRC_CLKSRC_GPIN0 = 0x4,
  CLK_SYS_AUXSRC_CLKSRC_GPIN1 = 0x5,
} clocks_clk_sys_ctrl_auxsrc_enum;

// --- Peripheral clock auxiliary source selection ---
typedef enum
{
  CLK_PERI_AUXSRC_CLK_SYS = 0x0,
  CLK_PERI_AUXSRC_CLKSRC_PLL_SYS = 0x1,
  CLK_PERI_AUXSRC_CLKSRC_PLL_USB = 0x2,
  CLK_PERI_AUXSRC_ROSC_CLKSRC_PH = 0x3,
  CLK_PERI_AUXSRC_XOSC_CLKSRC = 0x4,
  CLK_PERI_AUXSRC_CLKSRC_GPIN0 = 0x5,
  CLK_PERI_AUXSRC_CLKSRC_GPIN1 = 0x6,
} clocks_clk_peri_ctrl_auxsrc_enum;

// --- Get reference clock source ---
static inline clocks_clk_ref_ctrl_src_enum clocks_clk_ref_get_src(void)
{
  return REG_GET_FIELD(*clocks_clk_ref_ctrl, CLOCKS_CLK_REF_CTRL_SRC);
}

// --- Set reference clock source ---
static inline void clocks_clk_ref_set_src(clocks_clk_ref_ctrl_src_enum const src)
{
  REG_SET_FIELD(*clocks_clk_ref_ctrl, CLOCKS_CLK_REF_CTRL_SRC, src);
}

// --- Get system clock source ---
static inline clocks_clk_sys_ctrl_src_enum clocks_clk_sys_get_src(void)
{
  return REG_GET_FIELD(*clocks_clk_sys_ctrl, CLOCKS_CLK_SYS_CTRL_SRC);
}

// --- Set system clock source ---
static inline void clocks_clk_sys_set_src(clocks_clk_sys_ctrl_src_enum const src)
{
  REG_SET_FIELD(*clocks_clk_sys_ctrl, CLOCKS_CLK_SYS_CTRL_SRC, src);
}

// --- Get system clock auxiliary source ---
static inline clocks_clk_sys_ctrl_auxsrc_enum clocks_clk_sys_get_auxsrc(void)
{
  return REG_GET_FIELD(*clocks_clk_sys_ctrl, CLOCKS_CLK_SYS_CTRL_AUXSRC);
}

// --- Set system clock auxiliary source ---
static inline void clocks_clk_sys_set_auxsrc(clocks_clk_sys_ctrl_auxsrc_enum const src)
{
  REG_SET_FIELD(*clocks_clk_sys_ctrl, CLOCKS_CLK_SYS_CTRL_AUXSRC, src);
}

// --- Check if peripheral clock is enabled ---
static inline bool clocks_clk_peri_is_enabled(void)
{
  return REG_GET_FIELD(*clocks_clk_peri_ctrl, CLOCKS_CLK_PERI_CTRL_ENABLE) != 0;
}

// --- Enable/disable peripheral clock ---
static inline void clocks_clk_peri_set_enabled(bool const enabled)
{
  REG_SET_FIELD(*clocks_clk_peri_ctrl, CLOCKS_CLK_PERI_CTRL_ENABLE, enabled);
}

// --- Get peripheral clock auxiliary source ---
static inline clocks_clk_peri_ctrl_auxsrc_enum clocks_clk_peri_get_auxsrc(void)
{
  return REG_GET_FIELD(*clocks_clk_peri_ctrl, CLOCKS_CLK_PERI_CTRL_AUXSRC);
}

// --- Set peripheral clock auxiliary source ---
static inline void clocks_clk_peri_set_auxsrc(clocks_clk_peri_ctrl_auxsrc_enum const src)
{
  REG_SET_FIELD(*clocks_clk_peri_ctrl, CLOCKS_CLK_PERI_CTRL_AUXSRC, src);
}

#ifdef __cplusplus
}
#endif
