#include "cpu/exec/template-start.h"

#define instr jnp

static void do_execute() {
	DATA_TYPE_S tmp = op_src->val;
    // eip+tmp + length +1
    print_asm("jnp %x\n", cpu.eip + tmp + DATA_BYTE + 1);
    if(cpu.PF == 0) cpu.eip += tmp;
}

make_instr_helper(i);

#include "cpu/exec/template-end.h"
