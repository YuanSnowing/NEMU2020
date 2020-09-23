#include "cpu/exec/template-start.h"

#define instr push

static void do_execute() {
    // op_src->val = (DATA_TYPE_S)op_src->val;
    // // before push every time
    // reg_l(R_ESP) -= 4;
	// swaddr_write(reg_l(R_ESP), 4, op_src->val);
	// print_asm("push 0x%x\n", op_src->val);
    if (DATA_BYTE == 1) op_src -> val = (int8_t)op_src -> val;
    reg_l(R_ESP) -= 4;
    swaddr_write(reg_l(R_ESP), 4, op_src -> val);
    print_asm_template1();
}

// decode
// imm is 8 16 32
make_instr_helper(i)
// reg is 16 32
#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(r)
make_instr_helper(rm)
#endif
#include "cpu/exec/template-end.h"
