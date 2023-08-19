#include "cpu/instr.h"
/*
Put the implementations of `push' instructions here.
*/
static void instr_execute_1op()
{
    /*
    ELSE (* StackAddrSize = 32 *)
        IF OperandSize = 16
        THEN
    ESP ← ESP - 2;
    (SS:ESP) ← (SOURCE); (* word assignment *)
    ELSE
    ESP ← ESP - 4;
    (SS:ESP) ← (SOURCE); (* dword assignment *)
    FI;
    FI;*/
    operand_read(&opr_src);
    cpu.esp -= data_size / 8;
    opr_dest.addr = cpu.esp;
    opr_dest.sreg = SREG_DS;
    opr_dest.val = opr_src.val;
    opr_dest.type = OPR_MEM;
    operand_write(&opr_dest);
}

make_instr_impl_1op(push, r, v);
make_instr_impl_1op(push, rm, v);
make_instr_impl_1op(push, i, v);


make_instr_func(push_i_b){
	int len = 1;
	opr_src.data_size = 8;
	opr_src.type = OPR_IMM; 
	opr_src.sreg = SREG_CS; 
	opr_src.addr = eip + 1; 
	len += opr_src.data_size / 8;
	print_asm_1("push", "b", len, &opr_src);
	cpu.esp-=data_size/8;
	operand_read(&opr_src);
	opr_src.val=sign_ext(opr_src.val,8);
	opr_dest.data_size = data_size;
	opr_dest.type=OPR_MEM;
	opr_dest.sreg=SREG_DS;
	opr_dest.addr=cpu.esp;
	opr_dest.val=opr_src.val;
	operand_write(&opr_dest);
	return len;
}

#define PUSH(x)               \
    cpu.esp -= data_size / 8; \
    opr_dest.addr = cpu.esp;  \
    opr_dest.val = x;        \
    operand_write(&opr_dest)

make_instr_func(pusha)
{
    print_asm_0("pusha", "", 1);
    uint32_t temp = cpu.esp;
    opr_dest.type = OPR_MEM;
    opr_dest.sreg = SREG_DS;
    opr_dest.data_size = data_size;

    PUSH(cpu.eax);
    PUSH(cpu.ecx);
    PUSH(cpu.edx);
    PUSH(cpu.ebx);
    PUSH(temp);
    PUSH(cpu.ebp);
    PUSH(cpu.esi);
    PUSH(cpu.edi);
    return 1;
}
