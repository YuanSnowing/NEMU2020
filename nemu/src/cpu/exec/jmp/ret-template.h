#include "cpu/exec/template-start.h"

#define instr ret

make_helper(concat(ret_n_, SUFFIX)) {
	cpu.eip = MEM_R(reg_l(R_ESP));
    reg_l(R_ESP) += DATA_BYTE;
	print_asm("ret\n");
	return 1;
}

static void do_execute() {
	cpu.eip = MEM_R(REG(R_ESP));
	if (DATA_BYTE == 2) cpu.eip &= 0xffff;
	int haha = op_src->val;
	REG(R_ESP) += DATA_BYTE + haha;
	printf("jhhajh%d\n",haha);
	print_asm_template1();
}

make_instr_helper(i);

#include "cpu/exec/template-end.h"


