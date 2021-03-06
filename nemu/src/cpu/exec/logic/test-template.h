#include "cpu/exec/template-start.h"

#define instr test

static void do_execute () {
	DATA_TYPE result = op_dest->val & op_src->val;
    int len = DATA_BYTE*8;
    cpu.SF = result >> (len-1);
    if(result) cpu.ZF = 0;
    else cpu.ZF = 1;
    cpu.CF = cpu.OF = 0;
    result ^= result >> 4;
    result ^= result >> 2;
    result ^= result >> 1;
    cpu.PF = !(result&1);
	print_asm_template2();
}

make_instr_helper(i2a);
make_instr_helper(i2rm);
make_instr_helper(r2rm);

#include "cpu/exec/template-end.h"
