/*
The MIT License (MIT)

Copyright (c) 2016 silverx

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files (the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions:

The above copyright notice and this permission notice shall be included in
all copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
THE SOFTWARE.
*/

#include "project.h"
#include "drv_time.h"
#include "led.h"
#include "config.h"

#define LEDALL 15

void ledon( uint8_t val )
{
	GPIO_PIN_ADDR(LED1PORT, LED1PIN) = 1;
}

void ledoff( uint8_t val )
{
	GPIO_PIN_ADDR(LED1PORT, LED1PIN) = 0;
}

void ledflash( uint32_t period , int duty )
{
#if ( LED_NUMBER > 0 )	
	if ( gettime() % period > (period*duty)>>4 )
	{
		ledon(LEDALL);
	}
	else
	{
		ledoff(LEDALL);
	}
#endif	
}


int ledlevel = 0;

uint8_t led_pwm2( uint8_t pwmval)
{
static int loopcount = 0;
	
ledlevel = pwmval;
loopcount++;
loopcount&=0xF;
if ( ledlevel > loopcount )
{
ledon( 255);
}
else
{
ledoff( 255);
}
return ledlevel;
}

int ledlevel2 = 0;
unsigned long lastledtime;
float lastledbrightness = 0;

//#define DEBUG

#ifdef DEBUG
int debug_led;
#endif

#include "util.h"

// delta- sigma first order modulator.
uint8_t led_pwm( uint8_t pwmval)
{
	static float ds_integrator= 0;
	unsigned int time = gettime();
	unsigned int ledtime = time - lastledtime; 

	lastledtime = time;

	
	float desiredbrightness = pwmval*( 1.0f/ 15.0f);

//	limitf( &lastledbrightness, 2);

	limitf( &ds_integrator, 2);
	
	ds_integrator += (desiredbrightness - lastledbrightness)*ledtime* ( 1.0f  /(float) LOOPTIME);
	
	if ( ds_integrator > 0.49f ) 
		{
		ledon( 255);
			lastledbrightness = 1.0f;
			#ifdef DEBUG
			debug_led<<=1;
			debug_led|=1;
			#endif
		}
		else
		{
		ledoff( 255);
			lastledbrightness = 0;
			#ifdef DEBUG
			debug_led<<=1;
			debug_led&=0xFFFFFFFE;
			#endif
		}	
return 0;	
}









