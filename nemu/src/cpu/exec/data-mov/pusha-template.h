#include "cpu/exec/template-start.h"

#define instr pusha

#ifndef voidpush
#define voidpush
static void push(uint32_t val){
    reg_l(R_ESP) -= DATA_BYTE;
    swaddr_write(reg_l(R_ESP),DATA_BYTE,(DATA_TYPE)val,R_SS);
}
#endif

make_helper(concat(pusha_,SUFFIX)){
    
    DATA_TYPE tmp = REG(R_ESP);
    push(REG(R_EAX));printf("pusha");
    push(REG(R_ECX));
    push(REG(R_EDX));
    push(REG(R_EBX));
    push(tmp);
    push(REG(R_EBP));
    push(REG(R_ESI));
    push(REG(R_EDI));

    print_asm_template1();
    return 1;
}


#include "cpu/exec/template-end.h"