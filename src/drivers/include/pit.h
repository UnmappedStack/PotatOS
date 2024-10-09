#pragma once
#include <stdint.h>

void init_PIT();
void unlock_pit();
void lock_pit();
void pit_wait(uint64_t ms);