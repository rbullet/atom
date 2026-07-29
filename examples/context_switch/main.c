#include <stdint.h>
#include <stdlib.h>

#include "atom.h"

void* scheduler_test(void* arg)
{
  while (1)
  {
    uint32_t values[8];

    __asm volatile(
        "ldr r0, =0x44444444\n"
        "mov r4, r0\n"
        "ldr r0, =0x55555555\n"
        "mov r5, r0\n"
        "ldr r0, =0x66666666\n"
        "mov r6, r0\n"
        "ldr r0, =0x77777777\n"
        "mov r7, r0\n"
        "ldr r0, =0x88888888\n"
        "mov r8, r0\n"
        "ldr r0, =0x99999999\n"
        "mov r9, r0\n"
        "ldr r0, =0xAAAAAAAA\n"
        "mov r10, r0\n"
        "ldr r0, =0xBBBBBBBB\n"
        "mov r11, r0\n"
        :
        :
        : "r0", "r4", "r5", "r6", "r7",
          "r8", "r9", "r10", "r11");

    thread_yield();

    __asm volatile(
        "str r4, [%0, #0]\n"
        "str r5, [%0, #4]\n"
        "str r6, [%0, #8]\n"
        "str r7, [%0, #12]\n"
        "mov r1, r8\n"
        "str r1, [%0, #16]\n"
        "mov r1, r9\n"
        "str r1, [%0, #20]\n"
        "mov r1, r10\n"
        "str r1, [%0, #24]\n"
        "mov r1, r11\n"
        "str r1, [%0, #28]\n"
        :
        :
        "r"(values)
        :
        "r1", "memory");

    ATOM_ASSERT(values[0] == 0x44444444, "Register r4 value changed unexpectedly");
    ATOM_ASSERT(values[1] == 0x55555555, "Register r5 value changed unexpectedly");
    ATOM_ASSERT(values[2] == 0x66666666, "Register r6 value changed unexpectedly");
    ATOM_ASSERT(values[3] == 0x77777777, "Register r7 value changed unexpectedly");
    ATOM_ASSERT(values[4] == 0x88888888, "Register r8 value changed unexpectedly");
    ATOM_ASSERT(values[5] == 0x99999999, "Register r9 value changed unexpectedly");
    ATOM_ASSERT(values[6] == 0xAAAAAAAA, "Register r10 value changed unexpectedly");
    ATOM_ASSERT(values[7] == 0xBBBBBBBB, "Register r11 value changed unexpectedly");
  }
  return NULL;
}

int main(void)
{
  for (int i=0; i<100; i++)
  {
    thread_t* thread = malloc(sizeof(thread_t));
    uint32_t* stack = malloc(1024);
    thread_init(thread, stack, 1024, scheduler_test, NULL);
    thread_start(thread);
  }
  for (;;);
  return 0;
}