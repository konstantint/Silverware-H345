//
#include "project.h"
#include "drv_time.h"
#include "config.h"

void failloop( int val);

unsigned long lastticks;
unsigned long globalticks;
volatile unsigned long systickcount = 0;


#ifndef SYS_CLOCK_FREQ_HZ
#define SYS_CLOCK_FREQ_HZ 48000000
#warning SYS_CLOCK_FREQ_HZ not present
#endif

#define CYCLES_PER_SYSTICK SYS_CLOCK_FREQ_HZ/1000


volatile uint32_t _systick_count;

// SysTick IRQ invokes this once every ms
void SysTick_Handler() {
	_systick_count++;
}

void time_init()
{
		// Set up time counting
		_systick_count = 0;

		// Set up the interrupt to fire once every millisecond.
		SysTick_Config(CYCLES_PER_SYSTICK);
}

// return time in uS from start ( micros())
// called at least once per second or time will overflow
unsigned long gettime(void)
{
		return _systick_count*1000 + (CYCLES_PER_SYSTICK - SysTick->VAL)*1000/CYCLES_PER_SYSTICK;
}



#ifdef ENABLE_OVERCLOCK
// delay in uS
void delay(uint32_t data)
{
	volatile uint32_t count;
	count = data * 7;
	while (count--);
}
#else
// delay in uS
void delay(uint32_t data)
{
		// We divide by 10 in addition because that seems to produce more or less correct results
		// Apparently the empty while loop below actually takes about 10 cycles per iteration.
		uint32_t ops_per_usec = SYS_CLOCK_FREQ_HZ / 1000000 / 10;

		volatile uint32_t count;
		count = data * ops_per_usec;
		while (count--);}
#endif
