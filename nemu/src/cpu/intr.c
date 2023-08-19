#include "cpu/intr.h"
#include "cpu/instr.h"
#include "memory/memory.h"

void raise_intr(uint8_t intr_no)
{
#ifdef IA32_INTR
	/*
保护断点和程序状态：依次将EFLAGS、CS、EIP寄存器压栈；

关中断：当异常事件是外部中断时，清除EFLAGS中的IF位；否则不清除；

识别异常和中断事件并转相应的处理程序执行：根据指令或硬件给出的异常和中断类型号，查询中断描述符表（IDT）得到处理程序的入口地址并跳转执行；
*/
	// Trigger an exception/interrupt with 'intr_no'
	// 'intr_no' is the index to the IDT
	// Push EFLAGS, CS, and EIP
	// Find the IDT entry using 'intr_no'
	// Clear IF if it is an interrupt
	// Set EIP to the entry of the interrupt handler	
	uint32_t addr=(uint32_t)hw_mem+page_translate(segment_translate(cpu.idtr.base+8*intr_no,SREG_CS));
	GateDesc* gatedesc=(void*)addr;
	cpu.esp-=4;
	vaddr_write(cpu.esp,SREG_DS,4,cpu.eflags.val);
	cpu.esp-=4;
	vaddr_write(cpu.esp,SREG_DS,4,sign_ext(cpu.cs.val,data_size));
	cpu.esp-=4;
	vaddr_write(cpu.esp,SREG_DS,4,cpu.eip);
	if(gatedesc->type==0xe)
		cpu.eflags.IF=0;
	cpu.eip=(gatedesc->offset_31_16<<16)+gatedesc->offset_15_0;
#endif
}

void raise_sw_intr(uint8_t intr_no)
{
	// return address is the next instruction
	cpu.eip += 2;
	raise_intr(intr_no);
}
