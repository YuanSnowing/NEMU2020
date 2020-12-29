#include "common.h"
#include "burst.h"
#include "memory/cache.h"
#include "memory/tlb.h"
#include "cpu/reg.h"

uint32_t dram_read(hwaddr_t, size_t);
void dram_write(hwaddr_t, size_t, uint32_t);

int is_mmio(hwaddr_t addr);
uint32_t mmio_read(hwaddr_t addr, size_t len, int map_NO);
void mmio_write(hwaddr_t addr, size_t len, uint32_t data, int map_NO);

/* Memory accessing interfaces */
///////////////////////////////////////////////////////
uint32_t hwaddr_read(hwaddr_t addr, size_t len) {
	// printf("hwaddr read\n");
	int port = is_mmio(addr);
	if(~port) {
		// printf("hahah");
		return mmio_read(addr, len, port) & (~0u >> ((4 - len) << 3));
	}
	return dram_read(addr, len) & (~0u >> ((4 - len) << 3));
	
    /*int id, ling=0;
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
	// printf("eip: 0x%08x\teax: 0x%08x\tedi:0x%08x\n", cpu.eip, cpu.eax, cpu.edi);
	// assert(ans == retu);
	// printf("tot_time : %d\n", tot_time);

	return retu;
	// return dram_read(addr, len) & (~0u >> ((4 - len) << 3));*/
}
///////////////////////
void hwaddr_write(hwaddr_t addr, size_t len, uint32_t data) {
	// printf("hwaddr write\n");
	int port = is_mmio(addr);
	if(~port) {
		mmio_write(addr, len, data, port);
		return;
	}
	// printf("write %d\n", data);
	// uint32_t ans = hwaddr_read(addr, len);
	// write_cache(addr, len, data);
	// printf("hah%d",ans);
	dram_write(addr, len, data); 
}
/////////////////////////////////////////////////////


lnaddr_t seg_translate(swaddr_t addr, size_t len, uint8_t sreg){
	// printf("hahah");
	if (cpu.cr0.protect_enable == 0) return addr;
	return cpu.sreg[sreg].base + addr;
}

hwaddr_t page_translate(lnaddr_t addr){ 
	if(!cpu.cr0.protect_enable || !cpu.cr0.paging) return addr;
	// addr: dirctionary | page | offset
	uint32_t a,b,c, tmp;
	a = addr >> 22, b = (addr >> 12) & 0x3ff, c = addr & 0xfff;

	int id = read_tlb(addr);
	if(id != -1){
		// printf("hit !%d\n", id);
		return (tlb[id].data << TLB_BIAS_BIT) + c;
	}
	// dir yebiao, sec yebiao
	Page_info dir, sec;
	
	// get dir
	tmp = (cpu.cr3.page_directory_base << 12) + (a << 2);
	dir.val = hwaddr_read(tmp, 4);
	// get page 
	tmp = (dir.addr << 12) + (b << 2);
	sec.val =  hwaddr_read(tmp, 4);
	// test valid
	// printf("eip:0x%x\n", cpu.eip);
	printf("hahahh ??? addr 0x%x\n", addr);
	// if(addr == 0x20228089) printf("%d\n", is_mmio(addr));
	Assert(dir.p == 1, "dir present");
	Assert(sec.p == 1, "second present");
	write_tlb(addr, (sec.addr << 12) + c);
	return (sec.addr << 12) + c;
}
hwaddr_t cmd_page_translate(lnaddr_t addr){
	if(!cpu.cr0.protect_enable || !cpu.cr0.paging) return addr;
	// dir yebiao, sec yebiao
	Page_info dir, sec;
	// addr: dirctionary | page | offset
	uint32_t a,b,c, tmp;
	a = addr >> 22, b = (addr >> 12) & 0x3ff, c = addr & 0xfff;
	// get dir
	tmp = (cpu.cr3.page_directory_base << 12) + (a << 2);
	dir.val = hwaddr_read(tmp, 4);
	// get page 
	tmp = (dir.addr << 12) + (b << 2);
	sec.val =  hwaddr_read(tmp, 4);
	// test valid
	if(dir.p != 1) {
		printf("This is not a valid page!\n");
		return 0;
	}
	if(sec.p != 1) {
		printf("This is not a valid page!\n");
		return 0;
	}
	return (sec.addr << 12) + c;
}


uint32_t lnaddr_read(lnaddr_t addr, size_t len) {
#ifdef DEBUG
	// assert(len == 1 || len == 2 || len == 4);
#endif
// printf("read ln:%x\n",addr);
	uint32_t bia = addr & 0xfff; //low 12 bit
	if(bia + len - 1 > 0xfff){ // cross page boundary
		// Assert(0, "kua ye le");
		size_t l = 0xfff - bia + 1;
		uint32_t ar = lnaddr_read(addr,l);
		uint32_t al = lnaddr_read(addr + l,len - l);
		return (al << (l << 3)) | ar;
	}else{
		// if(addr == 0x8137) printf("here here~1\n");
		hwaddr_t hwaddr = page_translate(addr);
		return hwaddr_read(hwaddr, len);
	}
	// return hwaddr_read(addr, len);
}

void lnaddr_write(lnaddr_t addr, size_t len, uint32_t data) {
#ifdef DEBUG
	// assert(len == 1 || len == 2 || len == 4);
#endif
// printf("write ln:%x\n",addr);
	uint32_t bia = addr & 0xfff; //low 12 bit
	if(bia + len - 1 > 0xfff){ // cross page boundary
		// Assert(0, "kua ye le");
		size_t l = 0xfff - bia + 1;
		lnaddr_write(addr, l, data & ((1<<(l<<3))-1) );
		lnaddr_write(addr+l, len-l, data >> (l<<3) );
	}else{
		// if(addr == 0x8137) printf("here here~2\n");
		hwaddr_t hwaddr = page_translate(addr);
		hwaddr_write(hwaddr, len, data);
	}
	// hwaddr_write(addr, len, data);
}

uint32_t swaddr_read(swaddr_t addr, size_t len, uint8_t sreg) {
#ifdef DEBUG
	// assert(len == 1 || len == 2 || len == 4);
#endif
	lnaddr_t lnaddr = seg_translate(addr, len, sreg);
	// if(lnaddr == 0x8137) printf("here here~1\n 0x%x", addr);
	return lnaddr_read(lnaddr, len);
}

void swaddr_write(swaddr_t addr, size_t len, uint32_t data, uint8_t sreg) {
#ifdef DEBUG
	assert(len == 1 || len == 2 || len == 4);
#endif
	lnaddr_t lnaddr = seg_translate(addr, len, sreg);
	lnaddr_write(lnaddr, len, data);
}

