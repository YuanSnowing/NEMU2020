#include "cpu/exec/helper.h"
#include "cpu/decode/modrm.h"
#include "cpu/reg.h"
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

void raise_intr(uint8_t);

make_helper(intr) {
	int NO = instr_fetch(eip + 1, 1);
	cpu.eip += 2;
	print_asm("int %x",NO);
	raise_intr(NO);
	return 0;
}


uint32_t pop(){
    int ret = swaddr_read(reg_l(R_ESP),4, R_SS);
	swaddr_write(reg_l(R_ESP),4,0,R_SS);
    reg_l(R_ESP) += 4;
    return ret;
}

make_helper(iret) {
	if (cpu.cr0.protect_enable == 0){
		cpu.eip = pop();
		cpu.cs.selector = pop();
		cpu.EFLAGS = pop();
	}else{
		cpu.eip = pop();
		cpu.cs.selector = pop();
		cpu.EFLAGS = pop();
		sreg_set(R_CS);
	}
	print_asm("iret");
	// printf("hahah");
	return 1;
}

make_helper(cli) {
	cpu.IF = 0;
	print_asm("cli");
	return 1;
}