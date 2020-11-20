#include "trap.h"

const char str[] = "Hello, world!\n";

int main() {
	asm volatile (  "movl $4, %eax;"	// system call ID, 4 = SYS_write
					"movl $1, %ebx;"	// file descriptor, 1 = stdout
					"movl $str, %ecx;"	// buffer address
					"movl $14, %edx;");	// length
	set_bp();
	asm volatile("int $0x80");
	set_bp();
	HIT_GOOD_TRAP;
	return 0;
}
