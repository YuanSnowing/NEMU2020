#include "cpu/exec/template-start.h"

#define instr jbe

static void do_execute() {
	DATA_TYPE_S tmp = op_src->val;
    // eip+tmp + length +1
    print_asm("jbe %x\n", cpu.eip + tmp + DATA_BYTE + 1);
    if(cpu.CF == 1 || cpu.ZF == 1) cpu.eip += tmp;
}

make_instr_helper(i);

#include "cpu/exec/template-end.h"
