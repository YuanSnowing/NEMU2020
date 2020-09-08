#include "cpu/exec/helper.h"

make_helper(exec);

make_helper(operand_size) {
	ops_decoded.is_operand_size_16 = true; //是66就要赋值
	int instr_len = exec(eip + 1);
	ops_decoded.is_operand_size_16 = false; // 改回去
	return instr_len + 1;
}
