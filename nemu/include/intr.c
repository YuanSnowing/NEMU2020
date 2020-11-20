#include <setjmp.h>
#include "cpu/reg.h"
jmp_buf jbuf;
void raise_intr(uint8_t NO){
	/* TODO: Trigger an interrupt/exception with NO
	 * use NO to index the IDT
	 */

	Gate_info gate;
	idt_des = &gate;

   	lnaddr_t pid = cpu.idtr.base + (NO<<3);
   	idt_des->fst = lnaddr_read(pid, 4);
	idt_des->sec = lnaddr_read(pid+4, 4);

	reg_l(R_ESP) -= 4;
	swaddr_write(reg_l(R_ESP), 4, cpu.eip, R_SS);

	reg_l(R_ESP) -= 4;
	swaddr_write(reg_l(R_ESP), 4, cpu.cs.selector, R_SS);
	
	reg_l(R_ESP) -= 4;
	swaddr_write(reg_l(R_ESP), 4, cpu.EFLAGS, R_SS);

	cpu.cs.selector = idt_des -> segment;
	cpu.eip = cpu.cs.base + idt_des -> offset_15_0 + ((idt_des -> offset_31_16) << 16);
	
	sreg_set(R_CS);
	/* Jump back to cpu_exec() */
    longjmp(jbuf, 1);
}