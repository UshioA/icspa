#include "cpu/instr.h"
/*
Put the implementations of `leave' instructions here.
*/
/*
IF StackAddrSize = 16
THEN
    SP ← BP;
ELSE (* StackAddrSize = 32 *)
    ESP ← EBP;
FI;
IF OperandSize = 16
THEN
    BP ← Pop();
ELSE (* OperandSize = 32 *)
    EBP ← Pop();
FI;
*/

make_instr_func(leave)
{
    cpu.esp = cpu.ebp;
    OPERAND op;
    op.data_size = data_size;
    op.type = OPR_MEM;
    op.sreg = SREG_DS;
    op.addr = cpu.esp;
    operand_read(&op);
    cpu.esp += data_size/8;
    cpu.ebp = op.val;
    return 1;
}