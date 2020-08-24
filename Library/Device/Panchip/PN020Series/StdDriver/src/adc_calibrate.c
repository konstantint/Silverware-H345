/**************************************************************************//**
 * @file     adc_calibrate.c
 * @version  V1.01
 * $Date: 18/06/21 9:39a $
 * @brief    PN020 series adc_calibrate driver source file
 * @note
 * Copyright (C) 2018 Shanghai Panchip Microelectronics Co., Ltd.   All rights reserved.
 *****************************************************************************/
#include <stdio.h>
#include "PN020Series.h"
#include "adc_calibrate.h"
#include "sys.h"

#define ADC_LOG printf
/******************************************************************************
 * @brief	   检测ID是否校验过
 * @param[in]  校验值 uint32_t  v
 * @return	   0 sucess
			   1 fail
 * @history  - V1.0, 2017-08-08 linjinan
******************************************************************************/
uint32_t adc_check(uint32_t  v)
{
	uint32_t ret = 1;
    uint32_t num = 0;
	uint32_t temp = v;
    if(0==v) return 1;
	temp &=0xffffff; 
     while(temp)
    {
         num += temp &0x01;
         temp >>= 1;
    }
	 if((v>>24)==num)
	 ret = 0 ;
	 return ret ;
	 
 }
/******************************************************************************
 * @brief	   读取ADC通道n的值
 * @param[in]  	uint32_t BIT_N  通道值
				uint32_t *pdata 存储 ADC_CODE 12bit有效
 * @return	   	0 sucess
			   1 fail
 * @history  - V1.0, 2017-08-12 linjinan
******************************************************************************/
uint32_t DoGetADC(uint32_t BIT_N,uint32_t *pdata)
{
	uint8_t ret = 1;
	uint8_t count =5;
	uint8_t i =0;
	uint32_t sum_data = 0;
	uint16_t temp = 0;
	switch(BIT_N)
	{
		case BIT0:
            GPIO_DISABLE_DIGITAL_PATH(P5,BIT0);  
			SYS->P5_MFP = (SYS->P5_MFP & ~SYS_MFP_P53_Msk) | SYS_MFP_P53_ADC_CH0;	
			break;
		case BIT1:
            GPIO_DISABLE_DIGITAL_PATH(P1,BIT1);    
			SYS->P1_MFP = (SYS->P1_MFP & ~SYS_MFP_P10_Msk) | SYS_MFP_P10_ADC_CH1;		
			break;
		case BIT2:
			SYS->P1_MFP = (SYS->P1_MFP & ~SYS_MFP_P12_Msk) | SYS_MFP_P12_ADC_CH2;	
            GPIO_DISABLE_DIGITAL_PATH(P1,BIT2);   
			break;
		case BIT3:
			SYS->P1_MFP = (SYS->P1_MFP & ~SYS_MFP_P13_Msk) | SYS_MFP_P13_ADC_CH3;
            GPIO_DISABLE_DIGITAL_PATH(P1,BIT3);   
			break;
		case BIT4:
			SYS->P1_MFP = (SYS->P1_MFP & ~SYS_MFP_P14_Msk) | SYS_MFP_P14_ADC_CH4;	
            GPIO_DISABLE_DIGITAL_PATH(P1,BIT4);           
			break;
		case BIT5:
			SYS->P1_MFP = (SYS->P1_MFP & ~SYS_MFP_P15_Msk) | SYS_MFP_P15_ADC_CH5;
            GPIO_DISABLE_DIGITAL_PATH(P1,BIT5);   		
			break;
	}    
    

	CLK->APBCLK = 0xFFFFFFFF;
	ADC->EXTSMPT = 256;
	ADC->CTL2=0X50F0002; 
	ADC->CTL = 1;
	ADC->CHEN = BIT_N;
	for(i=0;i<1;i++)
	{

		ADC->CTL |=ADC_CTL_SWTRG_Msk;
		count = 1;
		while(count--)
		{	delay_us(100);
			if((ADC->STATUS&0X08)==0)
			{	ret =0;
			 	break;
			}
			else
			{	
				delay_us(100);
				ret =1;
			}
		}
		if(ret) return ret ;		
		sum_data += ADC->DAT;
		ADC->STATUS = ADC->STATUS;
	}	
	sum_data = 0;
	for(i=0;i<50;i++)
	{
		ADC->CTL |= ADC_CTL_SWTRG_Msk;
		count = 5;
		while(count--)
		{	delay_us(100);
			if((ADC->STATUS&0X08)==0)
			{	ret =0;
			 	break;
			}
			else
			{	
				delay_us(100);
				ret =1;
			}
		}
		if(ret) return ret ;
		temp =(ADC->DAT&0XFFF);
		sum_data += temp;
		ADC->STATUS = ADC->STATUS;
	}
	*pdata =(sum_data/50)&0xfff;
	return ret;
}

/******************************************************************************
 * @brief	ADC模型V=K*CODE ;V电压值； K:ADC比例系数 , Xcode表示adc转换结果code（0--4095）
            内部BGAP为恒定不变电压。Vbgap=k*CODE_bgap;芯片在出厂时给芯片供恒定电压2730mv，使用adc测试出BGAP值,记为BGAP_CODE_UID0;
            改变电源供电,测试BGAP电压记为bgap_code 。k=2730/4095*BGAP_CODE_UID0/bga_code 。 
            待测通道电压值为 V_x =X_CODE*k=X_CODE*2730/4095*BGAP_CODE_UID0/bga_code        
            芯片封装测试时AVDD=2730MV,读取BGAP 值保存在UCID0中
 * @in       k
 * @return  0 sucess
            1 fail
 * @history  - V1.0, 2017-07-28 linjinan
******************************************************************************/
#define VCC 2730
 uint32_t adc_get_k(float *k)
{
  



	uint32_t ucid0 = 0;
    uint32_t bgap_code = 0 ;
	SYS_UnlockReg();	
	FMC->ISPCTL =  0xfd; 
	ucid0 =FMC_ReadUCID(0);
//     ADC_LOG("UCID0[%X] \n",ucid0);	
    if(adc_check(ucid0))
    {
        ADC_LOG("ucid0=[0x%x] check error!\n ",ucid0);
       *k =(float)  VCC/4095 ; 
        return 1;      
    } 	
	if(DoGetADC(BIT9,&bgap_code))
	{	
 		ADC_LOG("get adc value fail!\r\n ");	
        return 2;
	}
  // 0.6667 = 2730(mv)/4095	,线上损耗70MV; 
     *k= (float)  VCC/4095* (ucid0 &0xfff)/bgap_code; 
 
    return 0 ;
  }

  
/******************************************************************************
 * @brief	Y=K*X+B; Y待测电压，x通道CODE值，b偏移量,k系数。通过测试内部电压和外部固定电压，求出K,B.
            ADC通道给个固定电压，比如ADC5输入电压为1250MV,
            读取通道和内部恒定电压BGAP 值
            内部BGAP为恒定不变电压，待测通道电压值为 V_x =X_CODE/B_CODE*UCID0_BGAP/4096*2730+b  芯片封装测试时AVDD2730MV,读取BGAP 值保存在UCID0
            
 * @return  0 sucess
            1 fail
 * @history  - V1.1, 2018-02-07 linjinan
******************************************************************************/

uint32_t  adc_calibrate(float *k,int *b)
{  

    if(adc_get_k(k))   //get k
    {
        ADC_LOG("get k fail! ");	
        return 1;
    }
         
    if(adc_get_b(b,*k))   //get b
    {
        ADC_LOG("get b fail! ");	
        return 2;
    }      
         
    return 0;


}


 /******************************************************************************
 * @brief	ADC 校准模式选择
 * @in      b，k
 * @return  0 sucess
            1 fail
******************************************************************************/
//  #define _4_LINE 4   //4线模式, 外部提供基准电压，    禁用PANLIK 进行ADC校准
//  #define _5_LINE 5   //5线模式，PANLINK提供基准电压，使能PANLINK进行ADC 校准
//  #define _8_LINE 8   //8线模式，PANLINK提供基准电压，使能PANLINK进行ADC 校准
//  #define LINE_MODE   _8_LINE // 5_LINE  4_LINE  8_LINE
 uint32_t adc_get_b(int  *b,float k)
{
#if (LINE_MODE==_4_LINE)
#define V_ADC5 1250   // 比如在ADC5_P15 提供一个基准电压为1250mv    
    uint32_t adc_code;
    if(DoGetADC(BIT5,&adc_code))
    {	
//         printf("get adc5 value fail ");	
        return 1;
    }else
    {
        *b=k*adc_code-V_ADC5;    // get b  
    }    
#elif(LINE_MODE==_5_LINE ||LINE_MODE== _8_LINE)

    uint32_t config1 = 0;
	SYS_UnlockReg();	
	FMC->ISPCTL =  0xfd; 
	config1 =FMC_Read(FMC_CONFIG_BASE+4);
    ADC_LOG("config1=[0x%x] \n ",config1);	
   
    if(adc_check(config1))
    {
        ADC_LOG("config1=[0x%x] check error!\n ",config1);	
        return 1;      
    } 
    
     *b=k*(config1&0xfff)-((config1>>12)&0xfff);  
  
   
 #endif           
    return 0 ;
  } 
  
  

 /*** (C) COPYRIGHT 2016 Shanghai Panchip Microelectronics Co., Ltd.   ***/
