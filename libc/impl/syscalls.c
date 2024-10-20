#include <syscall.h>

#define CALL_INTERRUPT_INSTRUCTION "int $0x80"

long syscall0(long n)
{
	unsigned long ret;
	__asm__ __volatile__ (CALL_INTERRUPT_INSTRUCTION : "=a"(ret) : "a"(n) : "rcx", "r11", "memory");
	return ret;
}

long syscall1(long n, long a1)
{
	unsigned long ret;
	__asm__ __volatile__ (CALL_INTERRUPT_INSTRUCTION : "=a"(ret) : "a"(n), "D"(a1) : "rcx", "r11", "memory");
	return ret;
}

long syscall2(long n, long a1, long a2)
{
	unsigned long ret;
	__asm__ __volatile__ (CALL_INTERRUPT_INSTRUCTION : "=a"(ret) : "a"(n), "D"(a1), "S"(a2)
						  : "rcx", "r11", "memory");
	return ret;
}

long syscall3(long n, long a1, long a2, long a3)
{
	unsigned long ret;
	__asm__ __volatile__ (CALL_INTERRUPT_INSTRUCTION : "=a"(ret) : "a"(n), "D"(a1), "S"(a2),
						  "d"(a3) : "rcx", "r11", "memory");
	return ret;
}

long syscall4(long n, long a1, long a2, long a3, long a4)
{
	unsigned long ret;
	register long r10 __asm__("r10") = a4;
	__asm__ __volatile__ (CALL_INTERRUPT_INSTRUCTION : "=a"(ret) : "a"(n), "D"(a1), "S"(a2),
						  "d"(a3), "r"(r10): "rcx", "r11", "memory");
	return ret;
}

long syscall5(long n, long a1, long a2, long a3, long a4, long a5)
{
	unsigned long ret;
	register long r10 __asm__("r10") = a4;
	register long r8 __asm__("r8") = a5;
	__asm__ __volatile__ (CALL_INTERRUPT_INSTRUCTION : "=a"(ret) : "a"(n), "D"(a1), "S"(a2),
						  "d"(a3), "r"(r10), "r"(r8) : "rcx", "r11", "memory");
	return ret;
}

long syscall6(long n, long a1, long a2, long a3, long a4, long a5, long a6)
{
	unsigned long ret;
	register long r10 __asm__("r10") = a4;
	register long r8 __asm__("r8") = a5;
	register long r9 __asm__("r9") = a6;
	__asm__ __volatile__ (CALL_INTERRUPT_INSTRUCTION : "=a"(ret) : "a"(n), "D"(a1), "S"(a2),
						  "d"(a3), "r"(r10), "r"(r8), "r"(r9) : "rcx", "r11", "memory");
	return ret;
}
