#include "cpu/exec/template-start.h"

#define instr lgdt

static void do_execute() {
	if (op_src -> size == 2)
		cpu.gdtr.base = swaddr_read(op_src -> addr + 2,3);
	else{
		cpu.gdtr.base = swaddr_read(op_src -> addr + 2,4);
	}
    cpu.gdtr.limit = swaddr_read(op_src -> addr,2);
	print_asm_template1();
}

make_instr_helper(rm);

#include "cpu/exec/template-end.h"