#ifndef __CACHE_H__
#define __CACHE_H__

#include "common.h"

/*在 NEMU 中实现一个 L2 cache, 它的性质如下:
⚫ 替换算法采用随机方式
⚫ write back
⚫ write allocate
你还需要在 restart()函数中对 cache 进行初始化, 将所有 valid bit 置为无效即
可. 把之前实现的 cache 作为 L1 cache, 修改它缺失的操作, 让它读写 L2
cache, 当 L2 cache 缺失时才读写 DRAM。*/
#define CACHE_BLOCK_BIT 6
#define CACHE_BLOCK_SIZE 64

#define CACHE_WAY_BIT_L1 3
#define CACHE_GROUP_BIT_L1 7
#define CACHE_WAY_SIZE_L1 (1<<CACHE_WAY_BIT_L1)
#define CACHE_GROUP_SIZE_L1 (1<<CACHE_GROUP_BIT_L1)
#define CACHE_SIZE_L1 (64 * 1024)

#define CACHE_WAY_BIT_L2 4
#define CACHE_GROUP_BIT_L2 12
#define CACHE_WAY_SIZE_L2 (1<<CACHE_WAY_BIT_L2)
#define CACHE_GROUP_SIZE_L2 (1<<CACHE_GROUP_BIT_L2)
#define CACHE_SIZE_L2 (4 * 1024 * 1024)

typedef struct{
    bool valid, dirty;
    uint32_t tag;
    uint8_t block[CACHE_BLOCK_SIZE];
}Cache;

Cache L1_Cache[CACHE_SIZE_L1 / CACHE_BLOCK_SIZE];
Cache L2_Cache[CACHE_SIZE_L2 / CACHE_BLOCK_SIZE];
void init_cache();
int read_cache(hwaddr_t addr);
void write_cache(hwaddr_t addr,size_t len, uint32_t data);

#endif