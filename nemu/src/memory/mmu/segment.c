#include "cpu/cpu.h"
#include "memory/memory.h"

// return the linear address from the virtual address and segment selector
uint32_t segment_translate(uint32_t offset, uint8_t sreg)
{
#ifdef IA32_SEG
	return offset+cpu.segReg[sreg].base;
#endif
    return -1;
}

// load the invisible part of a segment register
void load_sreg(uint8_t sreg)
{
    #ifdef IA32_SEG
	/* TODO: load the invisibile part of the segment register 'sreg' by reading the GDT.
	 * The visible part of 'sreg' should be assigned by mov or ljmp already.
	 */

	uint32_t addr = (uint32_t)hw_mem + cpu.gdtr.base + (cpu.segReg[sreg].index << 3);
	SegDesc* segdes = (void*)addr;
	uint32_t base = segdes->base_15_0 + (segdes->base_23_16 << 16) + (segdes->base_31_24 << 24);
	uint32_t limit = segdes->limit_15_0 + (segdes->limit_19_16 << 16);
	assert(base==0);
	assert(limit == 0xFFFFF);
	assert(segdes->granularity);
	cpu.segReg[sreg].base = base;
	cpu.segReg[sreg].limit = limit;
	#endif
}
