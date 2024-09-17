#pragma once
#include <stdatomic.h>
#define Spinlock atomic_flag

volatile void spinlock_aquire();
void spinlock_release();