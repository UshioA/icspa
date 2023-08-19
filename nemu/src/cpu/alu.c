#include "cpu/cpu.h"

inline uint32_t extract(uint32_t num, uint32_t data_size){
    return num&(0xFFFFFFFF >> (32 - data_size));
}

inline void set_CF_add(uint32_t result,uint32_t src,size_t data_size){
    result=sign_ext(result&(0xFFFFFFFF>>(32-data_size)),data_size);
    src=sign_ext(src&(0xFFFFFFFF>>(32-data_size)),data_size);
    cpu.eflags.CF=(result<src);
}

inline void set_CF_adc(uint32_t result, uint32_t src,size_t data_size){
    result = sign_ext(extract(result, data_size), data_size);
    src = sign_ext(extract(src,data_size),data_size);
    cpu.eflags.CF = (result < src) || (cpu.eflags.CF && !(result-src));
}

inline void set_PF(uint8_t num){
    int i = 0;
    while(num){
        num &= (num-1);
        i++;
    }
    cpu.eflags.PF=!(i&1);
}

inline void set_ZF(uint32_t num, size_t data_size){
    cpu.eflags.ZF = (num&(0xFFFFFFFF >> (32-data_size)))==0;
}

inline void set_SF(uint32_t num, size_t data_size){
    cpu.eflags.SF = sign(sign_ext(num &(0xFFFFFFFF >> (32-data_size)),data_size));
}

inline void set_OF(uint32_t num,uint32_t dest, uint32_t src, size_t data_size, bool plus){
    num = sign_ext(extract(num, data_size), data_size);
    src = sign_ext(extract(src, data_size), data_size);
    dest = sign_ext(extract(dest, data_size),data_size);
    if(plus){
        if(sign(dest) == sign(src)){
            cpu.eflags.OF = sign(dest)^sign(num);
        }else{
            cpu.eflags.OF = 0;
        }
    }else{
        if(sign(dest) == sign(src)){
            cpu.eflags.OF = 0;
        }else{
            cpu.eflags.OF = sign(dest) ^ sign(num);
        }
    }
}

inline uint32_t alu_add(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_add(src, dest, data_size);
#else
    uint32_t ret = src + dest;
    set_CF_add(ret, src, data_size);
    set_PF(ret);
    set_ZF(ret, data_size);
    set_SF(ret, data_size);
    set_OF(ret, dest, src,data_size,1);
    return ret&(0xFFFFFFFF >> (32-data_size));
#endif
}


inline uint32_t alu_adc(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_adc(src, dest, data_size);
#else
    uint32_t ret = dest + src + cpu.eflags.CF;
    set_CF_adc(ret, src, data_size);
    set_PF(ret);
    set_ZF(ret, data_size);
    set_SF(ret, data_size);
    set_OF(ret, dest, src,data_size,1);
    return ret&(0xFFFFFFFF >> (32-data_size));
#endif
}

inline uint32_t alu_sub(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_sub(src, dest, data_size);
#else
    uint32_t ret = dest - src;
    cpu.eflags.CF = extract(dest,data_size)<extract(src,data_size);
    set_PF(ret);
    set_ZF(ret,data_size);
    set_SF(ret,data_size);
    set_OF(ret, dest,src,data_size,0);
	return extract(ret,data_size);
#endif
}

inline uint32_t alu_sbb(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_sbb(src, dest, data_size);
#else
    uint32_t ret = dest - src - cpu.eflags.CF;
    cpu.eflags.CF = cpu.eflags.CF?extract(dest,data_size)<=extract(src,data_size):extract(dest,data_size)<extract(src,data_size);
    set_PF(ret);
    set_ZF(ret,data_size);
    set_SF(ret,data_size);
    set_OF(ret, dest,src,data_size,0);
    return extract(ret,data_size);
#endif
}

inline uint64_t alu_mul(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_mul(src, dest, data_size);
#else
    uint64_t xorNum = data_size == 32?0xffffffff00000000:data_size==16?0xffff0000:0xff00;
    uint64_t ret = (uint64_t)src*(uint64_t)dest;
    cpu.eflags.OF=(!!(ret&xorNum));
    cpu.eflags.CF=cpu.eflags.OF;
    return ret;
#endif
}

inline int64_t alu_imul(int32_t src, int32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_imul(src, dest, data_size);
#else
    uint64_t xorNum = data_size == 32?0xffffffff00000000:data_size==16?0xffff0000:0xff00;
    int64_t ret = (int64_t)src*(int64_t)dest;
    cpu.eflags.OF=(!!(ret&xorNum));
    cpu.eflags.CF=cpu.eflags.OF;
    return ret;
#endif
}

// need to implement alu_mod before testing
inline uint32_t alu_div(uint64_t src, uint64_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_div(src, dest, data_size);
#else
    uint64_t ret = (uint64_t)dest/(uint64_t)src;
    return ret;
#endif
}

// need to implement alu_imod before testing
inline int32_t alu_idiv(int64_t src, int64_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_idiv(src, dest, data_size);
#else
    return (int64_t)dest/(int64_t)src;
#endif
}

inline uint32_t alu_mod(uint64_t src, uint64_t dest)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_mod(src, dest);
#else
    return (uint32_t)dest%src;
#endif
}

inline int32_t alu_imod(int64_t src, int64_t dest)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_imod(src, dest);
#else
    return (int32_t)dest%src;
#endif
}

inline uint32_t alu_and(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_and(src, dest, data_size);
#else
    uint32_t ret = src&dest;
    cpu.eflags.CF = 0;    
    cpu.eflags.OF=0;
    set_PF(ret);
    set_ZF(ret,data_size);
    set_SF(ret,data_size);
    return extract(ret,data_size);
#endif
}

inline uint32_t alu_xor(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_xor(src, dest, data_size);
#else
    uint32_t ret = src^dest;
    cpu.eflags.CF = 0;    
    cpu.eflags.OF=0;
    set_PF(ret);
    set_ZF(ret,data_size);
    set_SF(ret,data_size);
    return extract(ret,data_size);
#endif
}

inline uint32_t alu_or(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_or(src, dest, data_size);
#else
    uint32_t ret = src|dest;
    cpu.eflags.CF = 0;    
    cpu.eflags.OF=0;
    set_PF(ret);
    set_ZF(ret,data_size);
    set_SF(ret,data_size);
    return extract(ret,data_size);
#endif
}

inline uint32_t alu_shl(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_shl(src, dest, data_size);
#else
    uint32_t ret = extract(dest,data_size);
    uint32_t xorNum = data_size == 32?0x80000000:data_size==16?0x8000:0x80;
    if(src){
        ret = ret << (src-1);
        cpu.eflags.CF = (ret & xorNum)==xorNum;
        ret = ret << 1;
    }
    set_PF(ret);
    set_ZF(ret,data_size);
    set_SF(ret,data_size);
    return extract(ret,data_size);
#endif
}

inline uint32_t alu_shr(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_shr(src, dest, data_size);
#else
    uint32_t ret = extract(dest,data_size);
    if(src){
        ret = ret >> (src-1);
        cpu.eflags.CF=(ret&0x1)==0x1;
        ret = ret>>1;
    }
    set_PF(ret);
    set_ZF(ret,data_size);
    set_SF(ret,data_size);
    return extract(ret,data_size);
#endif
}

inline uint32_t alu_sar(uint32_t src, uint32_t dest, size_t data_size) 
{
#ifdef NEMU_REF_ALU
	return __ref_alu_sar(src, dest, data_size);
#else
    uint32_t ret = extract(dest,data_size);
    uint32_t xorNum = data_size == 32?0x80000000:data_size==16?0x8000:0x80;
    if((ret&xorNum)!=xorNum){
        return alu_shr(src,dest,data_size);
    }
    if(src){
        while(src-1){
            src--;
            ret = ret >> 1;
            ret = ret ^ xorNum;
        }
        cpu.eflags.CF=(ret&0x1)==0x1;
        ret = ret>>1;
    }
    ret = ret^xorNum;
    set_PF(ret);
    set_ZF(ret,data_size);
    set_SF(ret,data_size);
    return extract(ret,data_size);
#endif
}

inline uint32_t alu_sal(uint32_t src, uint32_t dest, size_t data_size)
{
#ifdef NEMU_REF_ALU
	return __ref_alu_sal(src, dest, data_size);
#else
    return alu_shl(src,dest,data_size);
#endif
}
