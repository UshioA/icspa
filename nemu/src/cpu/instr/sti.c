#include "cpu/instr.h"
/*
Put the implementations of `sti' instructions here.
*/
make_instr_func(sti){
    print_asm_0("sti","",1);
    cpu.eflags.IF = 1;
    return 1;
}