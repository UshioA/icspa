#include "cpu/instr.h"
/*
Put the implementations of `iret' instructions here.
*/
make_instr_func(iret)
{
#ifdef IA32_INTR
    OPERAND r;
    r.sreg = SREG_SS;
    r.type = OPR_MEM;
    
    r.data_size = 32;
    r.addr = cpu.esp;
    operand_read(&r);
    cpu.eip = r.val;
    cpu.esp += 4;

    r.data_size = 32;
    r.addr = cpu.esp;
    operand_read(&r);
    cpu.cs.val = r.val & 0xffff;
    cpu.esp += 4;

    r.data_size = 32;
    r.addr = cpu.esp;
    operand_read(&r);
    cpu.eflags.val = r.val;
    cpu.esp += 4;
#endif
    return 0;
}