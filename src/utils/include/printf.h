#include <stdarg.h>

#include "../../processors/include/spinlock.h"

#pragma once

// colours
#define BLK "\e[0;30m"
#define RED "\e[0;31m"
#define GRN "\e[0;32m"
#define YEL "\e[0;33m"
#define BLU "\e[0;34m"
#define MAG "\e[0;35m"
#define CYN "\e[0;36m"
#define WHT "\e[0;37m"

// bold
#define BBLK "\e[1;30m"
#define BRED "\e[1;31m"
#define BGRN "\e[1;32m"
#define BYEL "\e[1;33m"
#define BBLU "\e[1;34m"
#define BMAG "\e[1;35m"
#define BCYN "\e[1;36m"
#define BWHT "\e[1;37m"

void aquire_serial_lock();

void write_text(char *text);
void printf_template(char* format, va_list args);
void kstatusf_helper(const char *format, ...);

#define ktestf(format, ...)          \
    do {                               \
        aquire_serial_lock();  \
        write_text(BCYN "[ TEST ] " BLK __FILE__ ": " WHT); \
        kstatusf_helper(format, ##__VA_ARGS__); \
    } while(0)

#define kstatusf(format, ...)          \
    do {                               \
        aquire_serial_lock();  \
        write_text(BYEL "[STATUS] " BLK __FILE__ ": " WHT); \
        kstatusf_helper(format, ##__VA_ARGS__); \
    } while(0)

#define kdebugf(format, ...)          \
    do {                               \
        aquire_serial_lock();  \
        write_text(BMAG "[KDEBUG] " BLK __FILE__ ": " WHT); \
        kstatusf_helper(format, ##__VA_ARGS__); \
    } while(0)

#define kfailf(format, ...)          \
    do {                               \
        aquire_serial_lock();  \
        write_text(BRED "[ FAIL ] " BLK __FILE__ ": " WHT); \
        kstatusf_helper(format, ##__VA_ARGS__); \
    } while(0)

void printf(char* format, ...);
void k_ok();