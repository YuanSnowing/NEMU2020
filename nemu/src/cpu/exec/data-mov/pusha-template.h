#include "cpu/exec/template-start.h"

#define instr pusha

// #ifndef mypush
// #define mypush
// static inline void push(uint32_t val){
//     printf("push %x at eip==%x\n", val, cpu.eip);
//     REG(R_ESP) -= DATA_BYTE;
//     printf("REG ESP: %x\n", REG(R_ESP));
//     swaddr_write(REG(R_ESP),DATA_BYTE,val,R_SS);
// }
// #endif

#if DATA_BYTE == 2
static void push_w(uint32_t val){
    REG(R_ESP) -= DATA_BYTE;
    printf("REG ESP: %x\n", REG(R_ESP));
    swaddr_write(REG(R_ESP),DATA_BYTE,val,R_SS);
}
#endif

#if DATA_BYTE == 4
static void push_l(uint32_t val){
    REG(R_ESP) -= DATA_BYTE;
    swaddr_write(REG(R_ESP),DATA_BYTE,val,R_SS);
}
#endif

make_helper(concat(pusha_,SUFFIX)){

    DATA_TYPE tmp = REG(R_ESP);
    concat(push_,SUFFIX)(REG(R_EAX));
    concat(push_,SUFFIX)(REG(R_ECX));
    concat(push_,SUFFIX)(REG(R_EDX));
    concat(push_,SUFFIX)(REG(R_EBX));
    concat(push_,SUFFIX)(tmp);
    concat(push_,SUFFIX)(REG(R_EBP));
    concat(push_,SUFFIX)(REG(R_ESI));
    concat(push_,SUFFIX)(REG(R_EDI));
    print_asm_template1();
    return 1;
}


#include "cpu/exec/template-end.h"