#ifndef __REG_H__
#define __REG_H__

#include "common.h"
#include "../../../lib-common/x86-inc/cpu.h"

enum { R_EAX, R_ECX, R_EDX, R_EBX, R_ESP, R_EBP, R_ESI, R_EDI };
enum { R_AX, R_CX, R_DX, R_BX, R_SP, R_BP, R_SI, R_DI };
enum { R_AL, R_CL, R_DL, R_BL, R_AH, R_CH, R_DH, R_BH };

/* TODO: Re-organize the `CPU_state' structure to match the register
 * encoding scheme in i386 instruction format. For example, if we
 * access cpu.gpr[3]._16, we will get the `bx' register; if we access
 * cpu.gpr[1]._8[1], we will get the 'ch' register. Hint: Use `union'.
 * For more details about the register encoding scheme, see i386 manual.
 */
typedef struct{
	uint16_t selector, attr;
	uint32_t base, limit;
}S_reg;

typedef struct{
	// regs
	union{
		struct{
			union {
				uint32_t _32;
				uint16_t _16;
				uint8_t _8[2];
			} gpr[8];
		};
		struct{
			uint32_t eax, ecx, edx, ebx, esp, ebp, esi, edi;
		};
	};
	// Eflags
	union{
		struct{
			uint32_t CF:1, :1,PF:1, :1, AF:1, :1, ZF:1, SF:1, TF:1, IF:1, DF:1, OF:1, IOPL:1, NT:1, :1,RF:1, VM:1, :14; 
		};
		uint32_t EFLAGS;
	};

	swaddr_t eip;

	// GDTR
	struct{
		uint32_t base,limit;
	}gdtr;

	CR0 cr0;

	union{
		struct{
			S_reg sreg[4];
		};
		struct{
			S_reg cs, ds, es, ss;
		};
	};
	
} CPU_state;

extern CPU_state cpu;

static inline int check_reg_index(int index) {
	assert(index >= 0 && index < 8);
	return index;
}

#define reg_l(index) (cpu.gpr[check_reg_index(index)]._32)
#define reg_w(index) (cpu.gpr[check_reg_index(index)]._16)
#define reg_b(index) (cpu.gpr[check_reg_index(index) & 0x3]._8[index >> 2])

extern const char* regsl[];
extern const char* regsw[];
extern const char* regsb[];

#endif
