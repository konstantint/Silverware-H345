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
    
 #define _4_LINE 4   //4线模式, 外部提供基准电压，    禁用PANLIK 进行ADC校准
 #define _5_LINE 5   //5线模式，PANLINK提供基准电压，使能PANLINK进行ADC 校准
 #define _8_LINE 8   //8线模式，PANLINK提供基准电压，使能PANLINK进行ADC 校准
 #define LINE_MODE   _8_LINE // 5_LINE  4_LINE  8_LINE    
    
    
 /*----------------------------------------------------------------------------
  Select  SYSCLK
 *----------------------------------------------------------------------------*/
      
#define _RF_48MHZ_MODE_     0   /*内部RF时钟 */  
#define _HXT_48MHZ_MODE_    1   /*MCU P51,P50脚外接16M晶振*/  
#define _HRILC_48MHZ_MODE_  2   /*内部48Mhz时钟*/  
#define _HRILC_55MHZ_MODE_  3   /*内部55Mhz时钟 */    
    
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
