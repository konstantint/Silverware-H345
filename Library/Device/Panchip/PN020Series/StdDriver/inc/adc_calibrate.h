/**************************************************************************//**
 * @file     adc_calibrate.h
 * @version  V1.00
 * $Revision: 3$
 * $Date: 16/02/29 14:50 $ 
 * @brief    PN020 series adc_calibrate driver header file
 *
 * @note
 * Copyright (C) 2016 Shanghai Panchip Microelectronics Co., Ltd.   All rights reserved.
 *****************************************************************************/ 
#ifndef __ADC_CALIBRATE_H__
#define __ADC_CALIBRATE_H__

#ifdef __cplusplus
extern "C"
{
#endif

uint32_t DoGetADC(uint32_t BIT_N,uint32_t *pdata);
uint32_t adc_get_b(int  *b,float k);
uint32_t  adc_calibrate(float *k,int *b);    
#endif //__ADC_CALIBRATE_H__

/*** (C) COPYRIGHT 2016 Shanghai Panchip Microelectronics Co., Ltd.   ***/

