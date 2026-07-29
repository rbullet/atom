#include "rp2040/system/clocks.h"

clocks_t* const clocks_clk_ref_ctrl = ((clocks_t* const)(CLOCKS_BASE + CLOCKS_CLK_REF_CTRL_OFFSET));
clocks_t* const clocks_clk_sys_ctrl = ((clocks_t* const)(CLOCKS_BASE + CLOCKS_CLK_SYS_CTRL_OFFSET));
clocks_t* const clocks_clk_peri_ctrl = ((clocks_t* const)(CLOCKS_BASE + CLOCKS_CLK_PERI_CTRL_OFFSET));