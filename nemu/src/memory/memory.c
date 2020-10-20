#include "common.h"
#include "burst.h"
#include "memory/cache.h"
#include "cpu/reg.h"

uint32_t dram_read(hwaddr_t, size_t);
void dram_write(hwaddr_t, size_t, uint32_t);


lnaddr_t seg_translate(swaddr_t addr, size_t len, uint8_t sreg){
	if (cpu.cr0.protect_enable == 0) return addr;
	return cpu.sreg[sreg].base + addr;
}

/* Memory accessing interfaces */
///////////////////////////////////////////////////////
uint32_t hwaddr_read(hwaddr_t addr, size_t len) {
    int id, ling=0;
	uint32_t bia = addr & (CACHE_BLOCK_SIZE - 1);
	uint8_t ret[2 * BURST_LEN];
	swaddr_write
	if(bia + len > CACHE_BLOCK_SIZE){// two block +
		id = read_cache(addr);
		memcpy(ret, L1_Cache[id].block + bia, CACHE_BLOCK_SIZE - bia);
		id = read_cache(addr + CACHE_BLOCK_SIZE - bia);
		memcpy(ret + CACHE_BLOCK_SIZE - bia, L1_Cache[id].block, len - (CACHE_BLOCK_SIZE - bia));
		// printf("ret two block!\n");
	}else{
		id = read_cache(addr);
		memcpy(ret, L1_Cache[id].block + bia, len);
	}
	// unalign_rw(addr, len);
	uint32_t retu = unalign_rw(ret+ling, 4) & (~0u >> ((4 - len) << 3));
	// printf("ret is %d\n", retu);
	// uint32_t ans = dram_read(addr, len) & (~0u >> ((4 - len) << 3));
	// printf("ret should be %d\n", ans);
	// assert(ans == retu);
	return retu;
	// return dram_read(addr, len) & (~0u >> ((4 - len) << 3));
}
///////////////////////
void hwaddr_write(hwaddr_t addr, size_t len, uint32_t data) {
	// printf("write %d\n", data);
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

uint32_t swaddr_read(swaddr_t addr, size_t len, uint8_t sreg) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	lnaddr_t lnaddr = seg_translate(addr, len, sreg);
	return lnaddr_read(lnaddr, len);
}

void swaddr_write(swaddr_t addr, size_t len, uint32_t data, uint8 sreg) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	lnaddr_t lnaddr = seg_translate(addr, len, sreg);
	lnaddr_write(lnaddr, len, data);
}

