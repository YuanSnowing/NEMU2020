#include <setjmp.h>
jmp_buf jbuf;

void raise_intr(uint8_t NO){
	/* TODO: Trigger an interrupt/exception with NO
	 * use NO to index the IDT
	 */
	reg_l(R_ESP) -= 4;
	swaddr_write(reg_l(R_ESP), 4, cpu.eip, R_SS);

	reg_l(R_ESP) -= 4;
	swaddr_write(reg_l(R_ESP), 4, cpu.cs.selector, R_SS);
	
	reg_l(R_ESP) -= 4;
	swaddr_write(reg_l(R_ESP), 4, cpu.EFLAGS, R_SS);
	
	/* Jump back to cpu_exec() */
    longjmp(jbuf, 1);
}