/**************************************************************************//**
 * @file     clk_calibrate.c
 * @version  V1.01
 * $Date: 18/06/21 9:39a $
 * @brief    PN020 series CLK_Calibrate driver source file
 * @note
 * Copyright (C) 2018 Shanghai Panchip Microelectronics Co., Ltd.   All rights reserved.
 *****************************************************************************/
#include "PN020Series.h"
#include "clk_calibrate.h"
#include "rf.h"
/*
*********************************************************************************************************
*	函 数 名: CLK_Calb_Timer1_Init
*	功能说明: 开启TIMER1 连续计数模式
*	形    参: 无 
*	返 回 值: 无
*********************************************************************************************************
*/ 
void CLK_Calb_Timer1_Init(void)
{
    uint32_t u32Prescale = 0; 
    uint32_t u32ns = 12000000;    
 	CLK_EnableModuleClock(TMR1_MODULE);    
	CLK_SetModuleClock(TMR1_MODULE, CLK_CLKSEL1_TMR1SEL_HCLK);         
     while(u32ns>=0x1000000)
    {
        u32ns >>=1;
        u32Prescale ++;
     }    
    TIMER1->CMP = u32ns;
    TIMER1->CTL = ((TIMER1->CTL & ~TIMER_CTL_PSC_Msk) |u32Prescale);
    TIMER1->CTL = (( TIMER1->CTL& ~TIMER_CTL_OPMODE_Msk)|TIMER_CONTINUOUS_MODE);   
    
 }
 
 
 /*
*********************************************************************************************************
*	函 数 名: CLK_Calb_SPI1_Init
*	功能说明: 初始化硬件SPI1 接口，用来控制xn297l
*	形    参: 无 
*	返 回 值: 无
*********************************************************************************************************
*/ 
void CLK_Calb_SPI1_Init(void)
{
    /* Setup SPI1 multi-function pin */
    SYS->P0_MFP |= SYS_MFP_P03_SPI1_SS;  //SPI1SS
    SYS->P1_MFP |=SYS_MFP_P11_SPI1_CLK|SYS_MFP_P16_SPI1_MOSI|SYS_MFP_P17_SPI1_MISO ;//SPI1 MOSI1 MISO1  
 
    GPIO_ENABLE_DIGITAL_PATH(P0,(1<<3));
    GPIO_ENABLE_DIGITAL_PATH(P1,(1<<1));   
    GPIO_ENABLE_DIGITAL_PATH(P1,(1<<6));
    GPIO_ENABLE_DIGITAL_PATH(P1,(1<<7));  
    CLK_EnableModuleClock(SPI1_MODULE);  
//     SYS->P0_MFP &= ~(SYS_MFP_P02_Msk); 
//     GPIO_SetMode(P0, BIT2, GPIO_MODE_OUTPUT);
    SPI_Open(SPI1, SPI_MASTER, SPI_MODE_0, 8, 50000);    
    delay_ms(500); //等待RF电源稳定
 }
/*
*********************************************************************************************************
*	函 数 名: CLK_Calibrate
*	功能说明:  使用内部RF 测试高频时钟
*	形    参: 无 
*	返 回 值: uint32_t  SystemCoreClock 。 返回内部HRLC 值
*********************************************************************************************************
*/  
 uint32_t  CLK_Calibrate(void)
 {
    register uint32_t io_cnt = 0;
    volatile uint32_t timer_cnt_var;    
    uint32_t i;   
    CLK_Calb_Timer1_Init();   
    CLK_Calb_SPI1_Init();
    CLK_Calb_RF_Init();     
    for(i = 0; i < 100000; i++);

    SYS->P2_MFP &= ~(SYS_MFP_P21_Msk);   
    GPIO_SetMode(P2, BIT1, GPIO_MODE_INPUT);
    TIMER1->CTL |= TIMER_CTL_CNTEN_Msk;
     
#pragma push
#pragma O2
    while(1){
        while(P21);
        while(!P21);
        io_cnt++;
        if(io_cnt > (100000-1)){
            timer_cnt_var = TIMER1->CNT;
            break;
        }
    }
#pragma pop  
    SYS_ResetModule(TMR1_RST,10);   //tmr1 rset
    SYS_ResetModule(SPI1_RST,10);  //spi1 reset
    RF_Reset();
    SYS_delay_10nop(100);
    return timer_cnt_var*20; // return SystemCore；
  }

  
/*
*********************************************************************************************************
*	函 数 名: SysClkInit
*	功能说明: 根据不同的时钟源产生系统时钟48mhz or 55Mhz
*	形    参: 无 
*	返 回 值: 无
*********************************************************************************************************
*/  
void SysClkInit(void)
{
    CLK_InitHIRC_Def();

#if _RF_48MHZ_MODE_==OSC_MODE 
    /* 内部RF做系统时钟源 */
    CLK_Calb_SPI1_Init();    //使能spi 控制rf       
    RF_Init();              // rf 输出4M时钟
    CLK_InitExtOSC4MPLL();  // 将4M时钟作为系统时钟倍频到48Mhz   
    
#elif  _HXT_48MHZ_MODE_==OSC_MODE
    /*外部16M 晶体做系统时钟源 */    
    CLK_InitHXTPLL();       
#elif (_HRILC_48MHZ_MODE_==OSC_MODE)|| (_HRILC_55MHZ_MODE_==OSC_MODE)
      /*内部高频时钟做系统时钟源 */  
    CLK_InitHIRC();  

    #if OSC_CALIB_EN > 0        
        SystemCoreClock=CLK_Calibrate();    
    #endif
    
#endif    
 }
 /*** (C) COPYRIGHT 2016 Shanghai Panchip Microelectronics Co., Ltd.   ***/
