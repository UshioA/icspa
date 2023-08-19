#include "memory/mmu/cache.h"
#include "memory/memory.h"
#include <time.h>
#include <stdlib.h>

#define SLOT_SIZE 64

#define CACHE_SIZE 1024

#define c 10
#define q 7
#define s 3
#define b 6
#define m (32 - b)
void hw_mem_write(paddr_t paddr, size_t len, uint32_t data);
uint32_t hw_mem_read(paddr_t paddr, size_t len);
typedef struct
{
	bool valid;
	uint32_t sign;			  
	uint8_t block[SLOT_SIZE]; 
							  
} CacheSlot;

CacheSlot Cache[CACHE_SIZE];

void init_cache()
{
	for (int i = 0; i < CACHE_SIZE; i++)
	{
		Cache[i].valid = 0;
	}
}

typedef union{
	struct
	{
		uint32_t addr:6;
		uint32_t grp_num:7;
		uint32_t sign : 19;
	};
	uint32_t val;
}paddr_u;

void cache_write(paddr_t paddr, size_t len, uint32_t data)
{
	hw_mem_write(paddr, len, data);
	paddr_u p;
	p.val = paddr;
	uint32_t mm_addr = p.addr; 
	uint32_t mm_grp_num = p.grp_num;
	uint32_t mm_sign = p.sign; 
	uint32_t cache_begin = mm_grp_num * 8, cache_end = (mm_grp_num + 1) * 8;
	for (uint32_t i = cache_begin; i < cache_end; i++)
	{
		if (Cache[i].sign == mm_sign && Cache[i].valid)
		{ 
			if (len + mm_addr <= SLOT_SIZE)
			{ 
				memcpy(Cache[i].block+mm_addr, &data, len);
			}
			else
			{ 
				cache_write(paddr, SLOT_SIZE - mm_addr, data);
				cache_write(paddr + SLOT_SIZE - mm_addr, len - SLOT_SIZE + mm_addr, data >> (8*(SLOT_SIZE - mm_addr)));
			}
			break;
		}
	}
}


uint32_t cache_read(paddr_t paddr, size_t len, bool* cross)
{
    *cross = false;
	uint32_t ret=0;
	paddr_u p;
	p.val = paddr;
	uint32_t mm_addr = p.addr; 
	uint32_t mm_grp_num = p.grp_num;
	uint32_t mm_sign = p.sign; 
	uint32_t cache_begin = mm_grp_num * 8, cache_end = (mm_grp_num + 1) * 8;
	for (uint32_t i = cache_begin; i < cache_end; i++)
	{
		if (Cache[i].valid==1 && Cache[i].sign == mm_sign)
		{ 
			if (len + mm_addr <= SLOT_SIZE)
			{ 
				memcpy(&ret, (char*)(Cache[i].block+mm_addr), len);
				if(ret != hw_mem_read(paddr,len)){
				    ret = hw_mem_read(paddr, len);
			        memcpy(Cache[i].block, (hw_mem + paddr - mm_addr), SLOT_SIZE);
			        return ret;
				}
				return ret;
			}
			else
			{ 
			    *cross = true;
			    bool dummy;
				uint32_t tmp1=0, tmp2=0;
				memcpy(&tmp1, Cache[i].block+mm_addr, SLOT_SIZE - mm_addr);
				uint32_t init = cache_read(paddr + SLOT_SIZE - mm_addr, len - SLOT_SIZE + mm_addr,&dummy); 
				tmp2 = init << ((SLOT_SIZE - mm_addr) * 8);								   
				if((tmp1|tmp2) != hw_mem_read(paddr,len)){
				    ret = hw_mem_read(paddr, len);
			        memcpy(Cache[i].block, (hw_mem + paddr - mm_addr), SLOT_SIZE);
			        return ret;
				}
				return tmp1 | tmp2;
			}
		}
	}
	ret = hw_mem_read(paddr, len);
	for (uint32_t i = cache_begin; i < cache_end; i++)
	{
		if (Cache[i].valid==0)
		{
			memcpy(Cache[i].block, (hw_mem + paddr - mm_addr), SLOT_SIZE);
			Cache[i].valid = 1;
			Cache[i].sign = mm_sign; 
			return ret;
		}
	}
	srand((unsigned)time(0));
	uint32_t i = rand() % 8;
	memcpy(Cache[cache_begin + i].block, (hw_mem+paddr - mm_addr), SLOT_SIZE);
	Cache[cache_begin + i].sign = mm_sign;
	return ret;
}
