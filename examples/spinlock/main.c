#include <stdio.h>
#include <string.h>
#include <atom.h>

#include "../../include/concurrent/spinlock.h"
#include "../../include/util/log.h"

#define BUFFER_SIZE 128

int main(void)
{
    // printf("Acquire spinlock. in %s\r\n", __func__);
    // printf("Acquire spinlock. %lu\r\n", __LINE__);
    // printf("Hello\r\n");

    log_info("Acquired spinlock %d", 12);
    log_info("Acquired spinlock");
    ATOM_ASSERT(0, "%s is not false", "hello");
    printf("\r\n");

    // log_info("Acquire spinlock.");
    // log_info("Hello\r\n");
    // spinlock_t* spinlock = spinlock_pool_reserve(SPINLOCK_POOLED);

    // spinlock_pool_return(spinlock);
    // spinlock_t* spinlock2 = spinlock_pool_reserve(SPINLOCK_POOLED);
    // log_info("Acquired spinlock: %p ", spinlock2);
    // printf("Acquire spinlock.\r\n");
    // ATOM_ASSERT(spinlock2 == spinlock, "TEST 1SE");
    // spinlock_pool_return(spinlock2);
    while (1);
    return 0;
}