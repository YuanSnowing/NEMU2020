#include "cpu/exec/template-start.h"

#define instr call
// static void do_execute () {}
// int call_i_SUFFIX (swaddr eip)
make_helper(concat(call_i_, SUFFIX)) {
	// eip pointing at 'call 0xe8',to get length of imm
	// len = decode_i_SUFFIX(cpu.eip + 1)
	int len = concat(decode_i_, SUFFIX)(cpu.eip + 1);
	// stack is from high addr to low addr, so esp should minus imm's value
	// databyte is up to suffix
	// esp is the top of stack, set it to the right place(length of call + imm)
	reg_l(R_ESP) -= DATA_BYTE;
	// write return address at esp, 
	// write(target, length, content)
	swaddr_write(reg_l(R_ESP), 4, eip+len, 0);
	// op_src->val = op_src->imm, 
	cpu.eip += op_src->val;
	print_asm("call 0x%x\n", cpu.eip+len+1);
	return len+1;
}

make_helper(concat(call_rm_, SUFFIX)) {
	int len = concat(decode_rm_, SUFFIX)(eip + 1);
	reg_l(R_ESP) -= DATA_BYTE;
	MEM_W(reg_l(R_ESP), cpu.eip + len, 0);
	cpu.eip = (DATA_TYPE_S)op_src->val - len - 1;
	print_asm("call 0x%x\n", cpu.eip+len+1);
	return len+1;
}

#include "cpu/exec/template-end.h"


