#include "cpu/instr.h"
/*
Put the implementations of `pop' instructions here.
*/
/*
IF StackAddrSize = 16
THEN
    IF OperandSize = 16
    THEN
        DEST ← (SS:SP); (* copy a word *)
        SP ← SP + 2;
    ELSE (* OperandSize = 32 *)
        DEST ← (SS:SP); (* copy a dword *)
        SP ← SP + 4;
    FI;
ELSE (* StackAddrSize = 32 * )
    IF OperandSize = 16
    THEN
        DEST ← (SS:ESP); (* copy a word *)
        ESP ← ESP + 2;
    ELSE (* OperandSize = 32 *)
        DEST ← (SS:ESP); (* copy a dword *)
        ESP ← ESP + 4;
    FI;
FI;
*/
static void instr_execute_1op()
{
    OPERAND dest;
    dest.addr = cpu.esp;
    dest.sreg = SREG_DS;
    dest.type = OPR_MEM;
    dest.data_size = data_size;
    operand_read(&dest);
    opr_src.val = dest.val;
    operand_write(&opr_src);
    cpu.esp += data_size / 8;
}

make_instr_impl_1op(pop,r,v);

#define POP(x)              \
    opr_src.addr = cpu.esp; \
    operand_read(&opr_src); \
    x = opr_src.val;        \
    cpu.esp += 32 / 8;

make_instr_func(popa)
{
    print_asm_0("popa", "", 1);
    opr_src.data_size = 32;
    opr_src.type = OPR_MEM;
    opr_src.sreg = SREG_DS;
    POP(cpu.edi);
    POP(cpu.esi);
    POP(cpu.ebp);
    POP(uint32_t throwaway);
    throwaway++;
    POP(cpu.ebx);
    POP(cpu.edx);
    POP(cpu.ecx);
    POP(cpu.eax);
    return 1;
}
