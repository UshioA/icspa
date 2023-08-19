#ifndef __INSTR_ADD_H__
#define __INSTR_ADD_H__
/*     __ref_add_r2rm_b, __ref_add_r2rm_v, __ref_add_rm2r_b, __ref_add_rm2r_v,
     __ref_add_i2a_b, __ref_add_i2a_v, inv, inv,*/
make_instr_func(add_r2rm_v);
make_instr_func(add_r2rm_b);
make_instr_func(add_rm2r_b);
make_instr_func(add_rm2r_v);
make_instr_func(add_i2a_b);
make_instr_func(add_i2a_v);
make_instr_func(add_i2rm_bv);
make_instr_func(add_i2rm_b);
make_instr_func(add_i2rm_v);

#endif
