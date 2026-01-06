#include "LPC17xx.h"
#include "systick.h"

void systick_init(uint32_t TimerInterval)
{
  SysTick->CTRL = 0;
	SysTick->LOAD = TimerInterval;
	SysTick->VAL = 0;
	SysTick->CTRL = 7;
}


