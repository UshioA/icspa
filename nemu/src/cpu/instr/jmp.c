#include "cpu/instr.h"

make_instr_func(jmp_near)
{
        OPERAND rel;
        rel.type = OPR_IMM;
        rel.sreg = SREG_CS;
        rel.data_size = data_size;
        rel.addr = eip + 1;

        operand_read(&rel);

        int offset = sign_ext(rel.val, data_size);
        // thank Ting Xu from CS'17 for finding this bug
        print_asm_1("jmp", "", 1 + data_size / 8, &rel);

        cpu.eip += offset;

        return 1 + data_size / 8;
}

make_instr_func(jmp_near_indirect)
{
        OPERAND rel;
        rel.data_size = data_size;

        int len = 1 + modrm_rm(eip + 1, &rel);        
        operand_read(&rel);
        print_asm_1("jmp", "", len, &rel);
        if (data_size == 16)
        {
                cpu.eip = rel.val & 0xffff;
        }
        else
        {
                cpu.eip = rel.val;
        }
        return 0;
}

make_instr_func(jmp_short){
    OPERAND rel;
    rel.type = OPR_IMM;
    rel.sreg = SREG_CS;
    rel.data_size = 8;
    rel.addr = eip+1;
    operand_read(&rel);
    print_asm_1("jmp","",2,&rel);
    cpu.eip += sign_ext(rel.val,rel.data_size);
    return 2;
}

make_instr_func(jmp_far_imm){
    OPERAND rel;
    rel.type = OPR_IMM;
    rel.sreg = SREG_CS;
    rel.data_size = 32;
    rel.addr = eip+1;
    operand_read(&rel);
    print_asm_1("jmp","",7,&rel);
    if(data_size==16){
        cpu.eip = rel.val & 0xffff;
    }else{
        cpu.eip = rel.val;
    }
#ifdef IA32_SEG
    cpu.segReg[1].val = rel.val;
    load_sreg(1);
#endif
    return 0;
}
    