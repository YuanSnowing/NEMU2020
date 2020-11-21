#include "cpu/exec/template-start.h"

#define instr ret

make_helper(concat(ret_n_, SUFFIX)) {
	cpu.eip = MEM_R(reg_l(R_ESP), R_SS);
    reg_l(R_ESP) += DATA_BYTE;
	print_asm("ret");
	return 1;
}

make_helper(concat(ret_i_, SUFFIX)){
	int haha = instr_fetch(eip + 1, DATA_BYTE), i;
	cpu.eip = MEM_R(REG(R_ESP), R_SS);
	if (DATA_BYTE == 2) cpu.eip &= 0xffff;
	REG(R_ESP) += DATA_BYTE;

	for (i = 0;i < haha; i += DATA_BYTE)
	    MEM_W(REG(R_ESP) + i,0, R_SS);
	
	REG(R_ESP) += haha;
	print_asm("ret\t$0x%x",haha);
	return 1;
}

#include "cpu/exec/template-end.h"


