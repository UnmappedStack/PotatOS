#pragma once

typedef enum {
    SYS_READ,
    SYS_WRITE,
    SYS_OPEN,
    SYS_CLOSE,
    SYS_SPAWN,
    SYS_GETCWD,
    SYS_POLL,
    SYS_PEEK,
    SYS_GETERRNO,
    SYS_EXIT,
    SYS_MMAP
} Syscall;

long syscall0(long n);
long syscall1(long n, long a1);
long syscall2(long n, long a1, long a2);
long syscall3(long n, long a1, long a2, long a3);
long syscall4(long n, long a1, long a2, long a3, long a4);
long syscall5(long n, long a1, long a2, long a3, long a4, long a5);
long syscall6(long n, long a1, long a2, long a3, long a4, long a5, long a6);
