#include <setjmp.h>
#include "cpu/reg.h"
extern jmp_buf jbuf;
static inline void push(int val){
	// printf("val:%x\n",val);
	reg_l(R_ESP) -= 4;
	swaddr_write(reg_l(R_ESP),4,val, R_SS);
}
void raise_intr(uint8_t NO){
	/* TODO: Trigger an interrupt/exception with ``NO''.
	 * That is, use ``NO'' to index the IDT.
	 */
	Assert((NO<<3)<=cpu.idtr.limit,"Interrupt NO tooo large!");
    
	Gate_info gat;
	idt_des = &gat;

	lnaddr_t addr = cpu.idtr.base + (NO << 3);
	idt_des -> fst = lnaddr_read(addr,4);
	idt_des -> sec = lnaddr_read(addr+4,4);
	
	push(cpu.EFLAGS);
	push(cpu.cs.selector);
	push(cpu.eip);
    printf("hahahahah");
	cpu.cs.selector = idt_des -> selector;

	sreg_set(R_CS);
	// printf("eip to %x\n", cpu.cs.base + idt_desc -> offset1 + (idt_desc -> offset2 << 16));
	cpu.eip = cpu.cs.base + idt_des -> offset1 + (idt_des -> offset2 << 16);
	 
    /* Jump back to cpu_exec() */
    longjmp(jbuf, 1);
}