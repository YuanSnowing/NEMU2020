#include "cpu/exec/template-start.h"

#define instr pop

static void do_execute() {
    if(DATA_BYTE != 1){
        OPERAND_W(op_src, swaddr_read(cpu.esp, DATA_BYTE));
        reg_l(R_ESP) += DATA_BYTE;
    }
	print_asm_template1();
}


// reg is 16 32
#if DATA_BYTE == 2 || DATA_BYTE == 4
make_instr_helper(r);
make_instr_helper(rm);
#endif

#include "cpu/exec/template-end.h"
