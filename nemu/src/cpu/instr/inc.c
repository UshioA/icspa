#include "cpu/instr.h"
/*
Put the implementations of `inc' instructions here.
*/
static void instr_execute_1op()
{
    operand_read(&opr_src);
    uint32_t i = cpu.eflags.CF;
    opr_src.val = alu_add(1, opr_src.val, data_size);
    operand_write(&opr_src);
    cpu.eflags.CF = i; // inc does not change CF;
}

make_instr_impl_1op(inc,r,v);
make_instr_impl_1op(inc,rm,v);