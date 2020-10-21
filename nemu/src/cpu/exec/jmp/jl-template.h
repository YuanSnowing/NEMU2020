#include "cpu/exec/template-start.h"

#define instr jl

static void do_execute() {
	DATA_TYPE_S tmp = op_src->val;
    // eip+tmp + length +1
    print_asm("jl %x", cpu.eip + tmp + DATA_BYTE + 1);
    if(cpu.ZF == 0 && cpu.SF != cpu.OF) cpu.eip += tmp;
}

make_instr_helper(i);

#include "cpu/exec/template-end.h"
