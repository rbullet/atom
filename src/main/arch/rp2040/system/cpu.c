
void cpu_breakpoint(void)
{
  __asm__ volatile("bkpt #0");
}
