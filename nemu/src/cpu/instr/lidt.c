#include "cpu/instr.h"
/*
Put the implementations of `lidt' instructions here.
*/
make_instr_func(lidt)
{
    #ifdef IA32_INTR
    uint32_t len = 1;
    OPERAND opr;
    len += modrm_rm(eip + 1, &opr);
    opr.data_size = 16;
    operand_read(&opr);
    cpu.idtr.limit = opr.val;
    opr.addr += 2;
    opr.data_size = 32;
    operand_read(&opr);
    cpu.idtr.base = opr.val;
    print_asm_1("lidt", "", len, &opr);
    return len;
    #endif
    return 1;
}