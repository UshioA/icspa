#include "cpu/instr.h"
/*
Put the implementations of `lea' instructions here.
*/
/*
IF OperandSize = 16 AND AddressSize = 16
THEN 
    r16 ← Addr(m);
ELSE
    IF OperandSize = 16 AND AddressSize = 32
    THEN
        r16 ← Truncate_to_16bits(Addr(m)); (* 32-bit address *)
    ELSE
        IF OperandSize = 32 AND AddressSize = 16
        THEN
            r32 ← Truncate_to_16bits(Addr(m));
        ELSE
            IF OperandSize = 32 AND AddressSize = 32
            THEN 
                r32 ← Addr(m);
            FI;
        FI;
    FI;
FI;
*/

make_instr_func(lea){
    opr_src.data_size=opr_dest.data_size=data_size;
    int len = 1 + modrm_r_rm(eip+1,&opr_dest,&opr_src);
    print_asm_2("lea",opr_src.data_size == 8 ? "b" : (opr_src.data_size == 16 ? "w" : "l"),len, &opr_src, &opr_dest);
    opr_dest.val = opr_src.addr;
    operand_write(&opr_dest);
    return len;
}