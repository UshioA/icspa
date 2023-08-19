#include "cpu/instr.h"
/*
Put the implementations of `lgdt' instructions here.
*/
/*
IF instruction = LIDT
THEN
    IF OperandSize = 16
    THEN 
        IDTR.Limit:Base ← m16:24 (* 24 bits of base loaded *)
    ELSE 
        IDTR.Limit:Base ← m16:32
    FI;
//ABOVE IS NO USE
ELSE (* instruction = LGDT *)
    IF OperandSize = 16
    THEN 
        GDTR.Limit:Base ← m16:24 (* 24 bits of base loaded *)
    ELSE 
        GDTR.Limit:Base ← m16:32;
    FI;
FI;
*/
make_instr_func(lgdt){
    uint32_t len = 0;
    #ifdef IA32_SEG
    OPERAND num;
    len = 1 + modrm_rm(eip+1,&num);
    num.data_size = 16;
    operand_read(&num);
    cpu.gdtr.limit = num.val;
    num.addr+=2;
    num.data_size = 32;
    operand_read(&num);
    cpu.gdtr.base = num.val;
    print_asm_1("lgdt", "", len,&num);
    #endif
    return len;
}