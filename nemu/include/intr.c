#include <setjmp.h>
#include "cpu/reg.h"
jmp_buf jbuf;
static inline void push(int val){
	printf("val:::%x\n", val);
	reg_l(R_ESP) -= 4;
	swaddr_write(reg_l(R_ESP),4,val,R_SS);
}
void raise_intr(uint8_t NO){
	/* TODO: Trigger an interrupt/exception with NO
	 * use NO to index the IDT
	 */

	Assert((NO<<3)<=cpu.idtr.limit,"Interrupt NO too large!");
	Gate_info gat;
	idt_des = &gat;

	lnaddr_t addr = cpu.idtr.base + (NO << 3);
	idt_des -> fst = lnaddr_read(addr,4);
	idt_des -> sec = lnaddr_read(addr+4,4);
	
	push(cpu.EFLAGS);
	push(cpu.cs.selector);
	push(cpu.eip);
	cpu.cs.selector = idt_des -> segment;
	sreg_set(R_CS);
	printf("eip to %x\n", cpu.cs.base + idt_des -> offset_15_0 + (idt_des -> offset_31_16 << 16));
	cpu.eip = cpu.cs.base + idt_des -> offset_15_0 + (idt_des -> offset_31_16 << 16);
	 /* Jump back to cpu_exec() */
    longjmp(jbuf, 1);
}