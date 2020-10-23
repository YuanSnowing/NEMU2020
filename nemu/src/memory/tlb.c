#include "memory/tlb.h"
#include "burst.h"
#include <time.h>
#include <stdlib.h>

void init_tlb(){
    int i;
    for(i = 0; i < TLB_SIZE; ++ i){
        tlb[i].valid = 0;
    }
    srand(time(0));
}
int read_tlb(lnaddr_t addr){
    int tag = addr >> TLB_BIAS_BIT;
    int i;
    for(i = 0; i < TLB_SIZE; ++ i){
        if(tlb[i].tag == tag && tlb[i].valid) return i;
    }
    return -1; // not hit
}

void write_tlb(lnaddr_t addr, hwaddr_t haaddr){
    int tag = addr >> TLB_BIAS_BIT, i;
    haaddr >>= TLB_BIAS_BIT;
    for(i = 0; i < TLB_SIZE; ++ i){
        if(!tlb[i].valid){
            tlb[i].tag = tag, tlb[i].data = haaddr, tlb[i].valid = 1;
        }
    }

    i = rand()%TLB_SIZE;
    tlb[i].tag = tag, tlb[i].data = haaddr, tlb[i].valid = 1;
    return;
}