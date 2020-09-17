#include "cpu/exec/template-start.h"

#define instr cmp

static void do_execute () {
	DATA_TYPE result = op_dest->val - op_src->val;
    int len = (DATA_BYTE<<3) - 1, a , b;
    a = (op_dest->val) >> len;
    b = (op_src ->val) >> len;
    cpu.ZF = !result;
    cpu.SF = result >> len;
	cpu.CF = op_dest->val < op_src->val;
    cpu.OF = (a != b && b == cpu.SF);
    result ^= result >> 4;
    result ^= result >> 2;
    result ^= result >> 1;
    cpu.PF = !(result&1);
	print_asm_template2();
}

#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(si2rm);
#endif

#include "cpu/exec/template-end.h"
