#include "cpu/exec/template-start.h"

make_helper(concat(movs_n_,SUFFIX)){
    swaddr_write(reg_l(R_EDI),4,swaddr_read(reg_l(R_ESI),4, R_DS), R_ES);
    if (cpu.DF == 0){
        reg_l(R_EDI) += DATA_BYTE;
        reg_l(R_ESI) += DATA_BYTE;
    }
    else{
        reg_l(R_EDI) -= DATA_BYTE;
        reg_l(R_ESI) -= DATA_BYTE;
    }
    print_asm("movs\n");
    return 1;
}

#include "cpu/exec/template-end.h"