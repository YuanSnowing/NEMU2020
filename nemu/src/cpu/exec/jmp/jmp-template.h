#include "cpu/exec/template-start.h"
    
#define instr jmp


static void do_execute() {
	DATA_TYPE_S tmp = op_src->val;
	if (op_src->type == OP_TYPE_IMM){
		cpu.eip += tmp;
	}
	else{
		cpu.eip = tmp - concat (decode_rm_,SUFFIX)(cpu.eip+1) -1;
	}
	print_asm_template1();
}
make_instr_helper(i)
make_instr_helper(rm)

#if DATA_BYTE == 4
extern Sreg_info *sreg_info;
Sreg_info tmp;
// make_helper(ljmp){
//     *sreg_info = tmp;
//     cpu.eip = instr_fetch(cpu.eip+1, 4) - 7;
//     cpu.cs.selector = instr_fetch(cpu.eip+1 + 4, 2);

//     uint16_t idx = cpu.cs.selector >> 3;//index of sreg
// 	lnaddr_t chart_addr = cpu.gdtr.base + (idx << 3);//chart addr
// 	(sreg_info->p1) = lnaddr_read(chart_addr, 4);
// 	(sreg_info->p2) = lnaddr_read(chart_addr + 4, 4);

// 	cpu.cs.base = (sreg_info->b1) + ((sreg_info->b2) << 16) + ((sreg_info->b3) << 24);
// 	cpu.cs.limit = (sreg_info->lim1) + ((sreg_info->lim2) << 16) + (0xfff << 24);
// 	if ((sreg_info->g) == 1) {	//g=0,1b; g=1,4kb, 2^12
// 		cpu.cs.limit <<= 12;
// 	}
// 	// printf("get here!\n");
//     print_asm("ljmp 0x%x 0x%x",instr_fetch(cpu.eip+1 + 4, 2),instr_fetch(cpu.eip+1, 4));
//     return 7;    
// }
make_helper(ljmp){
    sreg_info = &tmp;

    uint32_t op1 = instr_fetch(cpu.eip + 1,4);
    uint16_t op2 = instr_fetch(cpu.eip + 1 + 4,2);
    
    cpu.eip = op1 - 7;
    cpu.cs.selector = op2;
    uint16_t idx = cpu.cs.selector >> 3;//index of sreg
	uint32_t chart_addr = cpu.gdtr.base + (idx << 3);//chart addr
    
	sreg_info -> p1 = lnaddr_read(chart_addr, 4);
	sreg_info -> p2 = lnaddr_read(chart_addr + 4, 4);

    uint32_t btmp = 0;

	btmp |= ((uint32_t)sreg_info -> b1);
	btmp |= ((uint32_t)sreg_info -> b2)<< 16;
	btmp |= ((uint32_t)sreg_info -> b3)<< 24;
    cpu.cs.base = btmp;

	uint32_t ltmp = 0;
	ltmp |= ((uint32_t)sreg_info -> lim1);
	ltmp |= ((uint32_t)sreg_info -> lim2) << 16;
	ltmp |= ((uint32_t)0xfff) << 24;
    cpu.cs.limit = ltmp;


	if (sreg_info -> g == 1) cpu.cs.limit <<= 12;//G = 0, unit = 1B;G = 1, unit = 4KB
    print_asm("ljmp %x %x",op2,op1);
    return 1 + 6;    
}
#endif

#include "cpu/exec/template-end.h"