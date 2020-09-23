#include "cpu/exec/template-start.h"
    
#define instr movzx

static void do_execute_b() {
	DATA_TYPE_S result = op_src->val & 0xff;
	if (op_dest->size == 4) result = (uint32_t)result;
	else if (op_dest->size == 2) result = (uint16_t)result;
	else result = (uint32_t)result;
	OPERAND_W(op_dest, result);
	print_asm_template2();
}

static void do_execute_w() {
	DATA_TYPE_S result = op_src->val & 0xffff;
	if (op_dest->size == 4) result = (uint32_t)result;
	else if (op_dest->size == 2) result = (uint16_t)result;
	else result = (uint32_t)result;
	OPERAND_W(op_dest, result);
	print_asm_template2();
}

make_helper(movzx_rm2r_b){
    return idex(eip, decode_rm2r_b, do_execute_b);
}
make_helper(movzx_rm2r_w){
    return idex(eip, decode_rm2r_w, do_execute_w);
}
#include "cpu/exec/template-end.h"