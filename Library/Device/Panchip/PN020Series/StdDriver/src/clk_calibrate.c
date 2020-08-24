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
*	�� �� ��: CLK_Calb_Timer1_Init
*	����˵��: ����TIMER1 ��������ģʽ
*	��    ��: �� 
*	�� �� ֵ: ��
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
*	�� �� ��: CLK_Calb_SPI1_Init
*	����˵��: ��ʼ��Ӳ��SPI1 �ӿڣ���������xn297l
*	��    ��: �� 
*	�� �� ֵ: ��
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
    delay_ms(500); //�ȴ�RF��Դ�ȶ�
 }
/*
*********************************************************************************************************
*	�� �� ��: CLK_Calibrate
*	����˵��:  ʹ���ڲ�RF ���Ը�Ƶʱ��
*	��    ��: �� 
*	�� �� ֵ: uint32_t  SystemCoreClock �� �����ڲ�HRLC ֵ
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
    return timer_cnt_var*20; // return SystemCore��
  }

  
/*
*********************************************************************************************************
*	�� �� ��: SysClkInit
*	����˵��: ���ݲ�ͬ��ʱ��Դ����ϵͳʱ��48mhz or 55Mhz
*	��    ��: �� 
*	�� �� ֵ: ��
*********************************************************************************************************
*/  
void SysClkInit(void)
{
    CLK_InitHIRC_Def();

#if _RF_48MHZ_MODE_==OSC_MODE 
    /* �ڲ�RF��ϵͳʱ��Դ */
    CLK_Calb_SPI1_Init();    //ʹ��spi ����rf       
    RF_Init();              // rf ���4Mʱ��
    CLK_InitExtOSC4MPLL();  // ��4Mʱ����Ϊϵͳʱ�ӱ�Ƶ��48Mhz   
    
#elif  _HXT_48MHZ_MODE_==OSC_MODE
    /*�ⲿ16M ������ϵͳʱ��Դ */    
    CLK_InitHXTPLL();       
#elif (_HRILC_48MHZ_MODE_==OSC_MODE)|| (_HRILC_55MHZ_MODE_==OSC_MODE)
      /*�ڲ���Ƶʱ����ϵͳʱ��Դ */  
    CLK_InitHIRC();  

    #if OSC_CALIB_EN > 0        
        SystemCoreClock=CLK_Calibrate();    
    #endif
    
#endif    
 }
 /*** (C) COPYRIGHT 2016 Shanghai Panchip Microelectronics Co., Ltd.   ***/
