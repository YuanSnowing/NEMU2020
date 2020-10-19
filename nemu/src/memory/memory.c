#include "common.h"
#include "memory/cache.h"

uint32_t dram_read(hwaddr_t, size_t);
void dram_write(hwaddr_t, size_t, uint32_t);

/* Memory accessing interfaces */
///////////////////////////////////////////////////////
uint32_t hwaddr_read(hwaddr_t addr, size_t len) {
    uint32_t gid = (addr >> CACHE_BLOCK_BIT) & ((1 << CACHE_GROUP_BIT_L1) - 1);
    int g_size =  (1 << CACHE_WAY_BIT_L1), id, ret; // group size
	uint32_t bia = addr & (g_size - 1);
	gid = gid * g_size;
	id = read_cache(addr);
	ret = L1_Cache[gid].block[id];
	if(bia + len > g_size){// two block +
		id = read_cache(addr+g_size-bia);
		ret <<= g_size-bia;
		ret += L1_Cache[gid].block[id];
	}
	// unalign_rw(addr, len);
	// return (uint32_t)(unalign_rw(ret, 4) & (~0u >> ((4 - len) << 3)));
	return ret & (~0u >> ((4 - len) << 3));
	// return dram_read(addr, len) & (~0u >> ((4 - len) << 3));
}
///////////////////////
void hwaddr_write(hwaddr_t addr, size_t len, uint32_t data) {
	write_cache(addr, len, data);
	// dram_write(addr, len, data);
}
/////////////////////////////////////////////////////
uint32_t lnaddr_read(lnaddr_t addr, size_t len) {
	return hwaddr_read(addr, len);
}

void lnaddr_write(lnaddr_t addr, size_t len, uint32_t data) {
	hwaddr_write(addr, len, data);
}

uint32_t swaddr_read(swaddr_t addr, size_t len) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	return lnaddr_read(addr, len);
}

void swaddr_write(swaddr_t addr, size_t len, uint32_t data) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	lnaddr_write(addr, len, data);
}

