#include "nemu.h"
#include "cpu/cpu.h"
#include "memory/memory.h"
#include "device/mm_io.h"
#include <memory.h>
#include <stdio.h>

uint8_t hw_mem[MEM_SIZE_B];

uint32_t hw_mem_read(paddr_t paddr, size_t len)
{
#ifdef DEBUG
    if(paddr+len > MEM_SIZE_B){
        printf("paddr:%u len:%u\n", paddr, len);
        fflush(0);
        assert(0);
    }
#endif
	uint32_t ret = 0;
	memcpy(&ret, hw_mem + paddr, len);
	return ret;
}

void hw_mem_write(paddr_t paddr, size_t len, uint32_t data)
{
	memcpy(hw_mem + paddr, &data, len);
}

uint32_t paddr_read(paddr_t paddr, size_t len)
{
#ifdef HAS_DEVICE_VGA
	int no = is_mmio(paddr);
	if(no!=-1){
		return mmio_read(paddr, len, no);
	}
#endif

#ifdef CACHE_ENABLED
    bool cross;
    uint32_t ret = cache_read(paddr,len,&cross);
#ifdef DEBUG
    if(ret != hw_mem_read(paddr,len)){
        printf("Cache HW dismatch\nexpected : %x\ngot : %x\n", hw_mem_read(paddr,len),ret);
        printf("paddr:%x\nlen:%x\ncross:%d",paddr,len,cross);
        fflush(0);
        assert(0);
    };
#endif
	return ret;
#else
	uint32_t ret = 0;
	ret = hw_mem_read(paddr, len);
	return ret;
#endif
}

void paddr_write(paddr_t paddr, size_t len, uint32_t data)
{
#ifdef HAS_DEVICE_VGA
	int no = is_mmio(paddr);
	if (no != -1)
	{
		mmio_write(paddr, len, data, no);
		return;
	}
#endif
#ifdef CACHE_ENABLED
	cache_write(paddr, len, data);
#else
	hw_mem_write(paddr, len, data);
#endif
}
uint32_t laddr_read(laddr_t laddr, size_t len)
{
#ifdef IA32_PAGE
	if(cpu.cr0.pg && cpu.cr0.pe){
	    if((laddr >> 12) != ((laddr + len - 1) >> 12)){
	        uint32_t len1 = (((laddr >> 12) + 1) << 12) - laddr;
	        uint32_t low = paddr_read(page_translate(laddr), len1);
	        uint32_t high = paddr_read(page_translate((((laddr >> 12) + 1) << 12)), len - len1);
	        return (high << (8 * len1)) + low;
	    }
		laddr = page_translate(laddr);
	}
#endif
	return paddr_read(laddr, len);
}

void laddr_write(laddr_t laddr, size_t len, uint32_t data)
{
#ifdef IA32_PAGE
	if(cpu.cr0.pg && cpu.cr0.pe){
	    if((laddr >> 12) != ((laddr + len - 1) >> 12)){
	        uint32_t len1 = (((laddr >> 12) + 1) << 12) - laddr;
	        uint32_t low = data & (0xFFFFFFFF >> (32 - 8 * len1));
	        uint32_t high = data >> (32 - 8 * len1);
	        paddr_write(page_translate(laddr), len1, low);
	        paddr_write(page_translate((((laddr >> 12) + 1) << 12)), len - len1, high);
	        return;
	    }
		laddr = page_translate(laddr);
	}
#endif
	paddr_write(laddr, len, data);
}

uint32_t vaddr_read(vaddr_t vaddr, uint8_t sreg, size_t len)
{
	assert(len == 1 || len == 2 || len == 4);
	fflush(0);
#ifdef IA32_SEG
	if(cpu.cr0.pe){
		return laddr_read(segment_translate(vaddr, sreg),len);
	}
#endif
	return laddr_read(vaddr, len);
}

void vaddr_write(vaddr_t vaddr, uint8_t sreg, size_t len, uint32_t data)
{
	assert(len == 1 || len == 2 || len == 4);	
	fflush(0);
#ifdef IA32_SEG
	if(cpu.cr0.pe){
		laddr_write(segment_translate(vaddr, sreg),len,data);
	}
#endif
	laddr_write(vaddr, len, data);
}

void init_mem()
{
	// clear the memory on initiation
	memset(hw_mem, 0, MEM_SIZE_B);
#ifdef CACHE_ENABLED
	init_cache();
#endif
#ifdef TLB_ENABLED
	make_all_tlb();
	init_all_tlb();
#endif
}

uint32_t instr_fetch(vaddr_t vaddr, size_t len)
{
	assert(len == 1 || len == 2 || len == 4);
	return vaddr_read(vaddr, SREG_CS, len);
}

uint8_t *get_mem_addr()
{
	return hw_mem;
}
