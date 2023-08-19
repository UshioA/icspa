#include "cpu/cpu.h"
#include "memory/memory.h"
#ifdef IA32_PAGE
typedef union{
	uint32_t val;
	struct{
		uint32_t offset:12;
		uint32_t page:10;
		uint32_t dir:10;
	};
}laddr_u;

// translate from linear address to physical address
paddr_t page_translate(laddr_t laddr)
{
#ifndef TLB_ENABLED
	laddr_u l;
	l.val = laddr;
	uint32_t offset = l.offset;
	uint32_t dir = l.dir;
	uint32_t page = l.page;
	PDE* pde = (PDE*)(hw_mem + (cpu.cr3.pdbr << 12) + (dir << 2));
	PTE* pte = (PTE*)(hw_mem + (pde->page_frame << 12) + (page << 2));	
#ifdef DEBUG
	if(!pde->present){
	    printf("pde not present\n");
	    printf("address of pde : %.8x\n", (uint32_t)pde);
	    printf("address of pte : %.8x\n",(uint32_t)pte);
	    fflush(0);
	    printf("pde->val : %.8x\n", pde->val);
	    printf("pte->val : %.8x\n", pte->val);
	    printf("laddr : %.8x\n", laddr);
	    fflush(0);
	    assert(0);
	}
	if(!pte->present){
	    printf("pte not present\n");
	    printf("address of pde : %.8x\n", (uint32_t)pde);
	    printf("address of pte : %.8x\n",(uint32_t)pte);
	    printf("pde->val : %.8x\n", pde->val);
	    printf("pte->val : %.8x\n", pte->val);
	    printf("laddr : %.8u\n",laddr);
	    fflush(0);
	    assert(0);
	}
#endif
	uint32_t paddr = (pte->page_frame << 12) + offset;
	return paddr; 
#else
	return tlb_read(laddr) | (laddr & PAGE_MASK);
#endif
}
#endif