#include "memory/cache.h"
#include "burst.h"
#include <time.h>
#include <stdlib.h>

uint32_t dram_read(hwaddr_t addr, size_t len);
void dram_write(hwaddr_t addr, size_t len, uint32_t data);

void init_cache(){
    int i = 0;
    for(; i < CACHE_BLOCK_SIZE * CACHE_SIZE_L1; ++ i){
        L1_Cache[i].valid = 0;
    }
}

int read_cache(hwaddr_t addr){
    uint32_t tag = (addr >> (CACHE_BLOCK_BIT + CACHE_GROUP_BIT_L1));
    uint32_t gid = (addr >> CACHE_BLOCK_BIT) & ((1 << CACHE_GROUP_BIT_L1) - 1);
    int i, g_size =  (1 << CACHE_WAY_BIT_L1); // group size

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
        L1_Cache[id].block[i] = dram_read((addr >> CACHE_BLOCK_BIT << CACHE_BLOCK_BIT) + 8 * i, 8);
    }
    L1_Cache[id].tag = tag;
    L1_Cache[id].valid = 1;
    return id;
}

void write_cache(hwaddr_t addr,size_t len, uint32_t data){
    uint32_t tag = (addr >> (CACHE_BLOCK_BIT + CACHE_GROUP_BIT_L1));
    uint32_t gid = (addr >> CACHE_BLOCK_BIT) & ((1 << CACHE_GROUP_BIT_L1) - 1);
    int i, g_size =  (1 << CACHE_WAY_BIT_L1); // group size

    uint32_t bia = addr & (CACHE_BLOCK_SIZE - 1);

    gid = gid * g_size;
    for(i = gid; i < gid + g_size; ++ i){
        if(tag == L1_Cache[i].tag && L1_Cache[i].valid){
            // hit, write through, 把数据同时写到Cache和内存中；
            if(bia + len <= CACHE_BLOCK_SIZE){
                dram_write(addr, len, data);
                L1_Cache[i].block[bia] = data;
                return;
            }else{ // two block +
                write_cache(addr, CACHE_BLOCK_SIZE-bia, data);
                write_cache(addr+CACHE_BLOCK_SIZE-bia, len-CACHE_BLOCK_SIZE+bia, data >> (CACHE_BLOCK_SIZE-bia) );
                return;
            }
        }
    }
    // not hit, No write allocate：直接把要写的数据写入到内存中。
    dram_write(addr, len, data);
}