#include "cpu/exec/template-start.h"

#define instr popa

#ifndef voidpopa
#define voidpopa
static uint32_t pop(){
    uint32_t ret = swaddr_read(reg_l(R_ESP),DATA_BYTE, R_SS);
    swaddr_write(reg_l(R_ESP),DATA_BYTE,0,R_SS);
    reg_l(R_ESP) += DATA_BYTE;
    return ret;
}
#endif

make_helper(concat(popa_,SUFFIX)){
    printf("popopopoa");
    REG(R_EDI) = pop();
    REG(R_ESI) = pop();
    REG(R_EBP) = pop();
    REG(R_EBX) = pop(); // mei yong
    REG(R_EBX) = pop();
    REG(R_EDX) = pop();
    REG(R_ECX) = pop();
    REG(R_EAX) = pop();
    print_asm("popa");
    return 1;
}


#include "cpu/exec/template-end.h"