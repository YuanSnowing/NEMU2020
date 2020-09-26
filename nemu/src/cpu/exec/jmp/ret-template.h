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
	REG(R_ESP) += DATA_BYTE + op_src->val;
	print_asm_template1();
}

make_instr_helper(i);

#include "cpu/exec/template-end.h"


