#include "cpu/exec/helper.h"
#include "cpu/decode/modrm.h"
#include "intr.c"
make_helper(nop) {
	print_asm("nop");
	return 1;
}

make_helper(int3) {
	void do_int3();
	do_int3();
	print_asm("int3");

	return 1;
}

make_helper(lea) {
	ModR_M m;
	m.val = instr_fetch(eip + 1, 1);
	int len = load_addr(eip + 1, &m, op_src);
	reg_l(m.reg) = op_src->addr;

	print_asm("leal %s,%%%s", op_src->str, regsl[m.reg]);
	return 1 + len;
}

make_helper(std){
	cpu.DF = 1;
	print_asm("std");
	return 1;
}

make_helper(cld){
	cpu.DF = 0;
	print_asm("cld");
	return 1;
}
extern Gate_info *idt_des;
make_helper(intr) {
	int NO = instr_fetch(eip + 1, 1);
	cpu.eip += 2;
	print_asm("int %x",NO);
	raise_intr(NO);
	return 0;
}
int get(){
	uint32_t hah = swaddr_read (reg_l(R_ESP) , 4, R_SS);
	reg_l(R_ESP) += 4;
	return hah;
}
make_helper(iret){
	if (cpu.cr0.protect_enable == 0)
	{
		cpu.eip = get();
		cpu.cs.selector = get ();
		cpu.EFLAGS = get ();
	}
	else{
		cpu.eip = get();
		cpu.cs.selector = get ();
		cpu.EFLAGS = get();
		sreg_set(R_CS);
	}
	print_asm("iret");
	return 0;
}

make_helper(cli){
	cpu.IF = 0;
	return 1;
}
