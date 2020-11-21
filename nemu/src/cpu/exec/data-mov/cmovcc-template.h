#include "cpu/exec/template-start.h"
#ifndef eflagsupdate
#define eflagsupdate
static inline bool check_cc_b() {
	return cpu.CF;
}


static inline bool check_cc_e() {
	return cpu.ZF;
}

static inline bool check_cc_ne() {
	return !cpu.ZF;
}

static inline bool check_cc_be() {
	return cpu.CF | cpu.ZF;
}

static inline bool check_cc_a() {
	return !(cpu.CF | cpu.ZF);
}

static inline bool check_cc_s() {
	return cpu.SF;
}

static inline bool check_cc_ns() {
	return !cpu.SF;
}


static inline bool check_cc_l() {
	return cpu.SF ^ cpu.OF;
}

static inline bool check_cc_ge() {
	return !(cpu.SF ^ cpu.OF);
}

static inline bool check_cc_le() {
	return (cpu.SF ^ cpu.OF) | cpu.ZF;
}

static inline bool check_cc_g() {
	return !((cpu.SF ^ cpu.OF) | cpu.ZF);
}
#endif
#define make_cmovcc_helper(cc) \
	make_helper(concat4(cmov, cc, _, SUFFIX)) { \
		int len = concat(decode_rm2r_, SUFFIX)(eip + 1); \
		(concat(check_cc_, cc)() ? OPERAND_W(op_dest, op_src->val) : 0 ); \
		print_asm(str(concat(cmov, cc)) str(SUFFIX) " %s,%s", op_src->str, op_dest->str); \
		return len + 1; \
	}

//make_cmovcc_helper(o)
//make_cmovcc_helper(no)
//make_cmovcc_helper(b)
//make_cmovcc_helper(ae)
make_cmovcc_helper(e);
//make_cmovcc_helper(ne)
//make_cmovcc_helper(be)
//make_cmovcc_helper(a)
//make_cmovcc_helper(s)
//make_cmovcc_helper(ns)
//make_cmovcc_helper(p)
//make_cmovcc_helper(np)
//make_cmovcc_helper(l)
//make_cmovcc_helper(ge)
make_cmovcc_helper(le);
//make_cmovcc_helper(g)

#include "cpu/exec/template-end.h"
