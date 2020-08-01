#include "project.h"
#include "drv_gpio.h"
#include "config.h"

void gpio_init(void)
{
	// Init LED pin
	GPIO_SetMode((GPIO_T*)(P0_BASE + 0x40*LED1PORT), 1 << LED1PIN, GPIO_MODE_OUTPUT);
	GPIO_PIN_ADDR(LED1PORT, LED1PIN) = 0; // Turn led off
}



// init fpv pin separately because it may use SWDAT/SWCLK don't want to enable it right away
int gpio_init_fpv(void)
{

	return 0;
}




