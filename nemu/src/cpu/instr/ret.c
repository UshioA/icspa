#include "cpu/instr.h"
/*
Put the implementations of `ret' instructions here.
*/

/*
IF instruction = near RET
THEN;
    IF OperandSize = 16
    THEN
        IP ← Pop();
        EIP ← EIP AND 0000FFFFH;
    ELSE (* OperandSize = 32 *)
        EIP ← Pop();
    FI;
    IF instruction has immediate operand THEN eSP ← eSP + imm16; FI;
FI;
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
make_instr_func(ret_near){
    print_asm_0("ret","",1);
    OPERAND dest;
    dest.data_size =data_size;
    dest.type = OPR_MEM;
    dest.addr = cpu.esp;
    dest.sreg = SREG_DS;
    //pop
    operand_read(&dest);
    cpu.esp += data_size/8;
    //end pop
    if(data_size==16){
        cpu.eip &= 0xffff; 
    }else{
        cpu.eip = dest.val;
    }
    return 0;
}

make_instr_func(ret_near_imm16){
    print_asm_0("ret","",1);
    OPERAND dest;
    dest.data_size =data_size;
    dest.type = OPR_MEM;
    dest.addr = cpu.esp;
    dest.sreg = SREG_DS;
    //pop
    operand_read(&dest);
    cpu.esp += data_size/8;
    //end pop
    if(dest.data_size == 16){
        cpu.eip = dest.val;
        cpu.eip &= 0xffff; 
    }else{
        cpu.eip = dest.val;
    }
    OPERAND imm;
    imm.data_size = 16;
    imm.type = OPR_IMM;
    imm.addr=eip+1;
    imm.sreg = SREG_CS;
    operand_read(&imm);
    cpu.esp +=data_size/8+sign_ext(imm.val,imm.data_size);
    return 0;
}