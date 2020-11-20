#include "cpu/exec/template-start.h"

#define instr pusha

#ifndef voidpush
#define voidpush
static void push(uint32_t val){
    REG(R_ESP) -= DATA_BYTE;
    swaddr_write(REG(R_ESP),DATA_BYTE,val,R_SS);
}
#endif

make_helper(concat(pusha_,SUFFIX)){
    
    DATA_TYPE tmp = REG(R_ESP);
    push(REG(R_EAX));
    push(REG(R_ECX));
    push(REG(R_EDX));
    push(REG(R_EBX));printf("pusha");
    push(tmp);
    push(REG(R_EBP));
    push(REG(R_ESI));
    push(REG(R_EDI));

    print_asm_template1();
    return 1;
}


#include "cpu/exec/template-end.h"