#ifndef __CACHE_H__
#define __CACHE_H__

#include "common.h"

/*在 NEMU 中实现一个 cache, 它的性质如下:
⚫ 标志位只需要 valid bit 即可
⚫ 替换算法采用随机方式
⚫ write through
⚫ not write allocate
你还需要在 restart()函数中对 cache 进行初始化, 将所有 valid bit 置为无效即
可 。 实 现 后 , 修 改 nemu/src/memory/memory.c 中 的 hwaddr_read() 和
hwaddr_write()函数, 让它们读写 cache, 当 cache 缺失时才读写 DRAM。
我们建议你将 cache 实现成可配置的, 一份代码可以适用于各种参数的 cache,
以减少重复代码. 这也是对 cache 知识的一次很好的复习。*/
#define CACHE_BLOCK_BIT 6
#define CACHE_BLOCK_SIZE 64

#define CACHE_WAY_BIT_L1 3
#define CACHE_GROUP_BIT_L1 7
#define CACHE_SIZE_L1 (64 * 1024)


typedef struct{
    bool valid;
    uint32_t tag;
    uint8_t block[CACHE_BLOCK_SIZE];
}Cache;

Cache L1_Cache[CACHE_BLOCK_SIZE * CACHE_SIZE_L1];

void init_cache();
int read_cache(hwaddr_t addr);
void write_cache(hwaddr_t addr,size_t len, uint32_t data);

#endif