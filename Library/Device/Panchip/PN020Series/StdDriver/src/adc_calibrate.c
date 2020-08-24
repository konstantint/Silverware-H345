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
 * @brief	   ���ID�Ƿ�У���
 * @param[in]  У��ֵ uint32_t  v
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
 * @brief	   ��ȡADCͨ��n��ֵ
 * @param[in]  	uint32_t BIT_N  ͨ��ֵ
				uint32_t *pdata �洢 ADC_CODE 12bit��Ч
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
 * @brief	ADCģ��V=K*CODE ;V��ѹֵ�� K:ADC����ϵ�� , Xcode��ʾadcת�����code��0--4095��
            �ڲ�BGAPΪ�㶨�����ѹ��Vbgap=k*CODE_bgap;оƬ�ڳ���ʱ��оƬ���㶨��ѹ2730mv��ʹ��adc���Գ�BGAPֵ,��ΪBGAP_CODE_UID0;
            �ı��Դ����,����BGAP��ѹ��Ϊbgap_code ��k=2730/4095*BGAP_CODE_UID0/bga_code �� 
            ����ͨ����ѹֵΪ V_x =X_CODE*k=X_CODE*2730/4095*BGAP_CODE_UID0/bga_code        
            оƬ��װ����ʱAVDD=2730MV,��ȡBGAP ֵ������UCID0��
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
  // 0.6667 = 2730(mv)/4095	,�������70MV; 
     *k= (float)  VCC/4095* (ucid0 &0xfff)/bgap_code; 
 
    return 0 ;
  }

  
/******************************************************************************
 * @brief	Y=K*X+B; Y�����ѹ��xͨ��CODEֵ��bƫ����,kϵ����ͨ�������ڲ���ѹ���ⲿ�̶���ѹ�����K,B.
            ADCͨ�������̶���ѹ������ADC5�����ѹΪ1250MV,
            ��ȡͨ�����ڲ��㶨��ѹBGAP ֵ
            �ڲ�BGAPΪ�㶨�����ѹ������ͨ����ѹֵΪ V_x =X_CODE/B_CODE*UCID0_BGAP/4096*2730+b  оƬ��װ����ʱAVDD2730MV,��ȡBGAP ֵ������UCID0
            
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
 * @brief	ADC У׼ģʽѡ��
 * @in      b��k
 * @return  0 sucess
            1 fail
******************************************************************************/
//  #define _4_LINE 4   //4��ģʽ, �ⲿ�ṩ��׼��ѹ��    ����PANLIK ����ADCУ׼
//  #define _5_LINE 5   //5��ģʽ��PANLINK�ṩ��׼��ѹ��ʹ��PANLINK����ADC У׼
//  #define _8_LINE 8   //8��ģʽ��PANLINK�ṩ��׼��ѹ��ʹ��PANLINK����ADC У׼
//  #define LINE_MODE   _8_LINE // 5_LINE  4_LINE  8_LINE
 uint32_t adc_get_b(int  *b,float k)
{
#if (LINE_MODE==_4_LINE)
#define V_ADC5 1250   // ������ADC5_P15 �ṩһ����׼��ѹΪ1250mv    
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
