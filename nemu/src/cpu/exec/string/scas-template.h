#include "cpu/exec/template-start.h"

make_helper(concat(scas_, SUFFIX)) {
	swaddr_t a = REG(R_EAX);
	swaddr_t b = MEM_R(reg_l(R_EDI), R_ES);
	uint32_t result = a-b;
	if (cpu.DF == 0) reg_l(R_EDI) += DATA_BYTE;
	else reg_l(R_EDI) -= DATA_BYTE;
	
	int len = (DATA_BYTE << 3) - 1;
	cpu.ZF = !result;
	cpu.SF = result >> len;
	cpu.CF = a < b;
	int aa = a >> len;
	int bb = b >> len;
    cpu.OF = (aa != bb && bb == cpu.SF);
	result ^= result >> 4;
	result ^= result >> 2;
	result ^= result >> 1;
	cpu.PF = !(result&1);
    print_asm("scas%s", str(SUFFIX));
	return 1;
}

#include "cpu/exec/template-end.h"
