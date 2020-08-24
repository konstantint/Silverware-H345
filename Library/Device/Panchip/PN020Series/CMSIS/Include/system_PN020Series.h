/**************************************************************************//**
 * @file     system_PN020Series.h
 * @version  V1.00
 * $Revision: 2 $
 * $Date: 16/02/26 10:19a $
 * @brief    PN020 series system clock definition file
 *
 * @note
 * Copyright (C) 2016 Shanghai Panchip Microelectronics Co., Ltd.   All rights reserved.
 *****************************************************************************/


#ifndef __SYSTEM_PN020SERIES_H__
#define __SYSTEM_PN020SERIES_H__

#ifdef __cplusplus
extern "C" {
#endif

/*---------------------------------------------------------------------------------------------------------*/
/* Macro Definition                                                                                        */
/*---------------------------------------------------------------------------------------------------------*/
 /*----------------------------------------------------------------------------
   adc calibrate mode selet
 *----------------------------------------------------------------------------*/
    
 #define _4_LINE 4   //4��ģʽ, �ⲿ�ṩ��׼��ѹ��    ����PANLIK ����ADCУ׼
 #define _5_LINE 5   //5��ģʽ��PANLINK�ṩ��׼��ѹ��ʹ��PANLINK����ADC У׼
 #define _8_LINE 8   //8��ģʽ��PANLINK�ṩ��׼��ѹ��ʹ��PANLINK����ADC У׼
 #define LINE_MODE   _8_LINE // 5_LINE  4_LINE  8_LINE    
    
    
 /*----------------------------------------------------------------------------
  Select  SYSCLK
 *----------------------------------------------------------------------------*/
      
#define _RF_48MHZ_MODE_     0   /*�ڲ�RFʱ�� */  
#define _HXT_48MHZ_MODE_    1   /*MCU P51,P50�����16M����*/  
#define _HRILC_48MHZ_MODE_  2   /*�ڲ�48Mhzʱ��*/  
#define _HRILC_55MHZ_MODE_  3   /*�ڲ�55Mhzʱ�� */    
    
#define OSC_MODE    _RF_48MHZ_MODE_
    
#define OSC_CALIB_EN        1  /* Enable (1) or Disable (0) calibrate crystal oscillator  frequency */  
    
#if(_HRILC_55MHZ_MODE_==OSC_MODE)     
#define __HIRC_55M__
#endif    

    
 /*----------------------------------------------------------------------------
  Define SYSCLK
 *----------------------------------------------------------------------------*/
  
#define __XTAL4M        	(4000000UL)
#define __XTAL16M        	(16000000UL)   
#define __XTAL32K        	(32768UL)
#define __IRC10K         	(10000UL)
#define __XTAL            	__XTAL4M

#define	__EXT				__XTAL4M
#define __HXT             	__XTAL16M
#ifdef __HIRC_55M__
#define __HIRC           	(55000000ul)
#else
#define __HIRC           	(48000000ul)
#endif
#define	__LIRC				__IRC10K
#define	__PLL				(48000000UL)


extern uint32_t __HSI;
extern uint32_t SystemCoreClock;        /*!< System Clock Frequency (Core Clock) */
extern uint32_t CyclesPerUs;            /*!< Cycles per micro second */
extern uint32_t PllClock;               /*!< PLL Output Clock Frequency          */
/**
 * Update SystemCoreClock variable
 *
 * @param  None
 * @return None
 *
 * @brief  Updates the SystemCoreClock with current core Clock
 *         retrieved from CPU registers.
 */

extern void SystemCoreClockUpdate (void);
extern void SystemInit (void);

#ifdef __cplusplus
}
#endif

#endif  //__SYSTEM_PN020SERIES_H__


/*** (C) COPYRIGHT 2016 Shanghai Panchip Microelectronics Co., Ltd.   ***/
