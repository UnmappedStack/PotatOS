#include <stdatomic.h>
#include <stddef.h>
#include <stdbool.h>
#include <stdint.h>
#include "include/spinlock.h"
#include "../drivers/include/serial.h"

volatile void spinlock_aquire(Spinlock *lock) {
    while (atomic_flag_test_and_set(lock)) {
        __builtin_ia32_pause();
    }
}

void spinlock_release(Spinlock *lock) {
    atomic_flag_clear(lock);
}