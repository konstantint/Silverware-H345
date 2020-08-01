
#include "project.h"
#include "drv_pwm.h"
#include "config.h"
#include "binary.h"

#ifdef USE_PWM_DRIVER

#ifndef DISABLE_PWM_PINS


void pwm_init(void)
{
	    CLK_EnableModuleClock(PWMCH01_MODULE);
	    CLK_EnableModuleClock(PWMCH23_MODULE);
	    CLK_EnableModuleClock(PWMCH45_MODULE);
	    CLK_SetModuleClock(PWMCH01_MODULE, CLK_CLKSEL1_PWMCH01SEL_HCLK, 0);
	    CLK_SetModuleClock(PWMCH23_MODULE, CLK_CLKSEL1_PWMCH23SEL_HCLK, 0);
	    CLK_SetModuleClock(PWMCH45_MODULE, CLK_CLKSEL2_PWMCH45SEL_HCLK, 0);
	    for (int ch = 0; ch < 6; ch++)
	    	PWM_ConfigOutputChannel(PWM, ch, 30000, 0);
	
	    SYS_UnlockReg();
	    SYS->P2_MFP = (SYS->P2_MFP & ~SYS_MFP_P25_Msk) | SYS_MFP_P25_PWM0_CH3;
	    SYS->P2_MFP = (SYS->P2_MFP & ~SYS_MFP_P24_Msk) | SYS_MFP_P24_PWM0_CH2;
	    SYS->P1_MFP = (SYS->P1_MFP & ~SYS_MFP_P14_Msk) | SYS_MFP_P14_PWM0_CH4;
	    SYS->P1_MFP = (SYS->P1_MFP & ~SYS_MFP_P12_Msk) | SYS_MFP_P12_PWM0_CH0;
	    SYS_LockReg();
	
	    PWM_EnableOutput(PWM, B00011101);
	    PWM_Start(PWM, B00011101);	
}


extern int failsafe;
unsigned long motorbeeptime = 0;
int beepon = 0;
#include "drv_time.h"

#ifndef MOTOR_BEEPS_TIMEOUT
// default value if not defined elsewhere
#define MOTOR_BEEPS_TIMEOUT 30e6
#endif

#define MOTOR_BEEPS_PWM_ON 0.2
#define MOTOR_BEEPS_PWM_OFF 0.0

void motorbeep( void)
{
	if (failsafe)
	{
		unsigned long time = gettime();
		if (!motorbeeptime)
				motorbeeptime = time;
		else
			if ( time - motorbeeptime > MOTOR_BEEPS_TIMEOUT)
			{
				if (!beepon&&(time%2000000 < 125000))
				{
				for ( int i = 0 ; i <= 3 ; i++)
					{
					pwm_set( i , MOTOR_BEEPS_PWM_ON);
					beepon=1;				
					}
				}
				else
				{
				for ( int i = 0 ; i <= 3 ; i++)
					{
					pwm_set( i , MOTOR_BEEPS_PWM_OFF);
					beepon=0;				
					}
					
				}
				
			}
	}
	else
		motorbeeptime = 0;
}


#include  <math.h>

void pwm_set( uint8_t number , float pwmf)
{
	
int pwm = pwmf * 100 ;
	
if ( pwm < 0 ) pwm = 0;
if ( pwm > 100 ) pwm = 100;

	
  switch( number)
	{
		case 0:
	    PWM_ConfigOutputChannel(PWM, 2, 30000, pwm); break;
		case 1:
	    PWM_ConfigOutputChannel(PWM, 3, 30000, pwm); break;		
		case 2:
	    PWM_ConfigOutputChannel(PWM, 4, 30000, pwm); break;		
		case 3: 
	    PWM_ConfigOutputChannel(PWM, 0, 30000, pwm); break;		
		default:
			// handle error;
			//
		break;	
				
	}
	
}







#else
// pwm pins disabled
void pwm_init(void)
{
}

void pwm_set( uint8_t number , float pwm)
{
}

#endif 

#endif // end USE_PWM_DRIVER


