#include "common.h"
#include "burst.h"
#include "memory/cache.h"
#include "cpu/reg.h"

uint32_t dram_read(hwaddr_t, size_t);
void dram_write(hwaddr_t, size_t, uint32_t);





/* Memory accessing interfaces */
///////////////////////////////////////////////////////
uint32_t hwaddr_read(hwaddr_t addr, size_t len) {
    int id, ling=0;
	uint32_t bia = addr & (CACHE_BLOCK_SIZE - 1);
	uint8_t ret[2 * BURST_LEN];
	// swaddr_write
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
	// printf("tot_time : %d\n", tot_time);
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


lnaddr_t seg_translate(swaddr_t addr, size_t len, uint8_t sreg){
	if (cpu.cr0.protect_enable == 0) return addr;
	return cpu.sreg[sreg].base + addr;
}

hwaddr_t page_translate(lnaddr_t addr){
	// printf("hahahh?\n");
	if(!cpu.cr0.protect_enable || !cpu.cr0.paging) return addr;
	// printf("here!\n");
	// dir yebiao, sec yebiao
	Page_info dir, sec;
	// addr: dirctionary | page | offset
	uint32_t a,b,c, tmp;
	a = addr >> 22, b = (addr >> 12) & 0x3ff, c = addr & 0xfff;
	// get dir
	tmp = (cpu.cr3.page_directory_base << 12) + (a << 2);
	dir.val = hwaddr_read(tmp, 4);
	// printf("dir this position!");
	// get page 
	if(dir.p != 1)printf("hahah? 0x%x", tmp);
	tmp = (dir.addr << 12) + (b << 2);
	sec.val =  hwaddr_read(tmp, 4);
	// printf("secval %d\n", sec.val);
	// test valid
	
	Assert(dir.p == 1, "dirctionary present");
	Assert(sec.p == 1, "second present");
	return (sec.addr << 12) + c;
}



uint32_t lnaddr_read(lnaddr_t addr, size_t len) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	uint32_t bia = addr & 0xfff; //low 12 bit
	if(bia + len - 1 > 0xfff){ // cross page boundary
		Assert(0, "kua ye le");
	}else{
		hwaddr_t hwaddr = page_translate(addr);
		return hwaddr_read(hwaddr, len);
	}
	// return hwaddr_read(addr, len);
}

void lnaddr_write(lnaddr_t addr, size_t len, uint32_t data) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	uint32_t bia = addr & 0xfff; //low 12 bit
	if(bia + len - 1 > 0xfff){ // cross page boundary
		Assert(0, "kua ye le");
	}else{
		hwaddr_t hwaddr = page_translate(addr);
		hwaddr_write(hwaddr, len, data);
	}
	// hwaddr_write(addr, len, data);
}

uint32_t swaddr_read(swaddr_t addr, size_t len, uint8_t sreg) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	lnaddr_t lnaddr = seg_translate(addr, len, sreg);
	return lnaddr_read(lnaddr, len);
}

void swaddr_write(swaddr_t addr, size_t len, uint32_t data, uint8_t sreg) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	lnaddr_t lnaddr = seg_translate(addr, len, sreg);
	lnaddr_write(lnaddr, len, data);
}

