#include "cpu/instr.h"
/*
Put the implementations of `call' instructions here.
*/

make_instr_func(call_near)
{
    /*
    IF rel16 or rel32 type of call
    THEN (* near relative call *)
        IF OperandSize = 16
        THEN
            Push(IP);
            EIP ← (EIP + rel16) AND 0000FFFFH;
        ELSE (* OperandSize = 32 *)
            Push(EIP);
            EIP ← EIP + rel32;
        FI;
    FI;
    */
	OPERAND rel,mem;
	rel.data_size=data_size;
    rel.type=OPR_IMM;
	rel.sreg=SREG_CS;
    rel.addr=cpu.eip+1;
    operand_read(&rel);
	print_asm_1("call", "", 1+data_size/8, &rel);
	cpu.esp-=data_size/8;
	mem.data_size=data_size;
	mem.type=OPR_MEM;
	mem.sreg=SREG_DS;
	mem.addr=cpu.esp;
	mem.val=cpu.eip+1+data_size/8;
	operand_write(&mem);
	int offset=sign_ext(rel.val, data_size);
	cpu.eip+=offset+1+data_size/8;
	return 0;
}


/*
Page 276 of 421
IF r/m16 or r/m32 type of call
THEN (* near absolute call *)
IF OperandSize = 16
THEN
Push(IP);
EIP ← [r/m16] AND 0000FFFFH;
ELSE (* OperandSize = 32 *)
Push(EIP);
EIP ← [r/m32];
FI;
FI;
*/

make_instr_func(call_near_indirect){
	int len=1;
	OPERAND rel,mem;
	rel.data_size=data_size;
    len+=modrm_rm(eip+1, &rel);
    operand_read(&rel);
	print_asm_1("call", "", len, &rel);
	cpu.esp-=data_size/8;
	mem.data_size=data_size;
	mem.type=OPR_MEM;
	mem.sreg=SREG_DS;
	mem.addr=cpu.esp;
	mem.val=cpu.eip+len;
	operand_write(&mem);
	if(data_size==16)
		cpu.eip=rel.val&0xFFFF;
	else
		cpu.eip=rel.val;
	return 0;
}
