#include "memory/cache.h"
#include "burst.h"
#include <time.h>
#include <stdlib.h>

void ddr3_read_my(hwaddr_t addr, void *data);

void init_cache(){
    int i = 0;
    for(; i < CACHE_BLOCK_SIZE * CACHE_SIZE_L1; ++ i){
        L1_Cache[i].valid = 0;
    }
}

int read_cache(hwaddr_t addr){
    uint32_t tag = (addr >> (CACHE_BLOCK_BIT + CACHE_GROUP_BIT_L1));
    uint32_t gid = (addr >> CACHE_BLOCK_BIT) & ((1 << CACHE_GROUP_BIT_L1) - 1);
    int i, g_size =  (1 << CACHE_WAY_BIT_L1);

    gid = gid * g_size;
    for(i = gid; i < gid + g_size; ++ i){
        if(tag == L1_Cache[i].tag && L1_Cache[i].valid){
            return i;
        }
    }

    // not hit
    srand(time(0));
    int id = gid + rand() % g_size;
    for(i = 0; i < CACHE_BLOCK_SIZE / 8; ++ i){
        ddr3_read_my((addr >> CACHE_BLOCK_BIT << CACHE_BLOCK_BIT) + 8 * i, L1_Cache[id].block+i*8);
    }
    L1_Cache[id].tag = tag;
    L1_Cache[id].valid = 1;
    return id;
}