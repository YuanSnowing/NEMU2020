#include "memory/cache.h"
#include "burst.h"
#include <time.h>
#include <stdlib.h>

void dram_write(hwaddr_t addr, size_t len, uint32_t data);
void snow_ddr3_read(hwaddr_t addr, void* data);
void snow_ddr3_write(hwaddr_t addr, void *data, uint8_t *mask);
void init_cache(){
    int i;
    for(i = 0; i < CACHE_SIZE_L1 / CACHE_BLOCK_SIZE; ++ i){
        L1_Cache[i].valid = 0;
    }
    for(i = 0; i < CACHE_SIZE_L2 / CACHE_BLOCK_SIZE; ++ i){
        L2_Cache[i].valid = L2_Cache[i].dirty = 0;
    }
}
int read_L2(hwaddr_t addr){
    uint32_t tag = (addr >> (CACHE_BLOCK_BIT + CACHE_GROUP_BIT_L2));
    uint32_t gidd = (addr >> CACHE_BLOCK_BIT) & (CACHE_GROUP_SIZE_L2 - 1);
    int i, gid;

    gid = gidd * CACHE_WAY_SIZE_L2;
    for(i = gid; i < gid + CACHE_WAY_SIZE_L2; ++ i){
        if(tag == L2_Cache[i].tag && L2_Cache[i].valid){
            return i;
        }
    }

    // not hit
    srand(time(0));
    int id = gid + rand() % CACHE_WAY_SIZE_L2;
    addr = (addr >> CACHE_BLOCK_BIT) << CACHE_BLOCK_BIT;
    // write back
    if(L2_Cache[id].valid && L2_Cache[id].dirty){
        uint8_t ret[2 * BURST_LEN];
        memset(ret, 1, sizeof ret);
        uint32_t st = (L2_Cache[id].tag << (CACHE_GROUP_BIT_L2+CACHE_BLOCK_BIT)) | (gid << CACHE_BLOCK_BIT);
        for(i = 0; i < CACHE_BLOCK_SIZE/BURST_LEN; ++ i){
            snow_ddr3_write(st + BURST_LEN*i, L2_Cache[id].block + BURST_LEN*i, ret);
        }
    }
    // update
    for(i = 0; i < CACHE_BLOCK_SIZE / BURST_LEN; ++ i){
        snow_ddr3_read(addr+BURST_LEN*i, L2_Cache[id].block + BURST_LEN * i);
    }
    L2_Cache[id].tag = tag;
    L2_Cache[id].valid = 1;
    L2_Cache[id].dirty = 0;
    return id;
}
int read_cache(hwaddr_t addr){
    uint32_t tag = (addr >> (CACHE_BLOCK_BIT + CACHE_GROUP_BIT_L1));
    uint32_t gid = (addr >> CACHE_BLOCK_BIT) & (CACHE_GROUP_SIZE_L1 - 1);
    int i;

    gid = gid * CACHE_WAY_SIZE_L1;
    for(i = gid; i < gid + CACHE_WAY_SIZE_L1; ++ i){
        if(tag == L1_Cache[i].tag && L1_Cache[i].valid){
            return i;
        }
    }

    // not hit
    srand(time(0));
    int id = gid + rand() % CACHE_WAY_SIZE_L1;
    int ans = read_L2(addr);
    memcpy(L1_Cache[id].block, L2_Cache[ans].block, CACHE_BLOCK_SIZE);


    L1_Cache[id].tag = tag;
    L1_Cache[id].valid = 1;
    return id;
}

void write_L2(hwaddr_t addr,size_t len, uint32_t data){
    uint32_t tag = (addr >> (CACHE_BLOCK_BIT + CACHE_GROUP_BIT_L2));
    uint32_t gid = (addr >> CACHE_BLOCK_BIT) & (CACHE_GROUP_SIZE_L2 - 1);
    int i;

    uint32_t bia = addr & (CACHE_BLOCK_SIZE - 1);

    gid = gid * CACHE_WAY_SIZE_L2;
    for(i = gid; i < gid + CACHE_WAY_SIZE_L2; ++ i){
        if(tag == L2_Cache[i].tag && L2_Cache[i].valid){
            // hit, write through, 把数据同时写到Cache和内存中；
            L2_Cache[i].dirty = 1;
            if(bia + len <= CACHE_BLOCK_SIZE){
                memcpy(L2_Cache[i].block + bia, &data, len);
            }else{ // two block +
                memcpy(L2_Cache[i].block+bia, &data, CACHE_BLOCK_SIZE-bia);
                write_L2(addr+CACHE_BLOCK_SIZE-bia, len-CACHE_BLOCK_SIZE+bia, data >> (CACHE_BLOCK_SIZE-bia) );
            }
            return;
        }
    }
    // not hit, write allocate
    int id = read_L2(addr);
    L2_Cache[id].dirty = 1;
    memcpy(L2_Cache[id].block + bia, &data, len);
}


void write_cache(hwaddr_t addr,size_t len, uint32_t data){
    uint32_t tag = (addr >> (CACHE_BLOCK_BIT + CACHE_GROUP_BIT_L1));
    uint32_t gid = (addr >> CACHE_BLOCK_BIT) & (CACHE_GROUP_SIZE_L1 - 1);
    int i;

    uint32_t bia = addr & (CACHE_BLOCK_SIZE - 1);

    gid = gid * CACHE_WAY_SIZE_L1;
    for(i = gid; i < gid + CACHE_WAY_SIZE_L1; ++ i){
        if(tag == L1_Cache[i].tag && L1_Cache[i].valid){
            // hit, write through, 把数据同时写到Cache和内存中；
            if(bia + len <= CACHE_BLOCK_SIZE){
                dram_write(addr, len, data);
                memcpy(L1_Cache[i].block + bia, &data, len);
                write_L2(addr, len, data);
            }else{ // two block +
                dram_write(addr,CACHE_BLOCK_SIZE - bia, data);
                memcpy(L1_Cache[i].block+bia, &data, CACHE_BLOCK_SIZE-bia);
                write_L2(addr,CACHE_BLOCK_SIZE - bia, data);
                // overleft
                write_cache(addr+CACHE_BLOCK_SIZE-bia, len-CACHE_BLOCK_SIZE+bia, data >> (CACHE_BLOCK_SIZE-bia) );
            }
            return;
        }
    }
    // not hit, write allocate
    write_L2(addr, len, data);
}