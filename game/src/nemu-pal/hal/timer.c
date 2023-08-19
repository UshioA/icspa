#include "hal.h"

static volatile uint32_t jiffy = 0;
static int fps = 0;
static int nr_draw = 0;

void incr_nr_draw(void)
{
	nr_draw++;
}

int get_fps()
{
	return fps;
}

void timer_event(void)
{
	++jiffy;
	if (!(jiffy & (HZ >> 1)))
	{
		fps = (nr_draw << 1) + 1;
		nr_draw = 0;
	}
}

uint32_t SDL_GetTicks()
{
    return 10*jiffy;
}

void SDL_Delay(uint32_t ms)
{
    uint32_t after = SDL_GetTicks() + ms;
    while(SDL_GetTicks() < after){
    }
    return;
}
