/*
 * adc_etcm.h
 *
 *  Created on: Nov 1, 2019
 *      Author: willc
 */

#ifndef ADC_ETCM_H_
#define ADC_ETCM_H_

#include "adc.h"
#include "device.h"

void initADC(uint32_t base);
//void initEPWM(void);
void initADCSOC(uint32_t base, ADC_Channel adc_channel, ADC_SOCNumber adcsoc0, ADC_SOCNumber adcsoc1, ADC_IntNumber adc_int);
uint16_t getADCVal(uint32_t base, uint32_t resultbase, ADC_SOCNumber adcsoc0, ADC_SOCNumber adcsoc1, ADC_IntNumber adc_int);

#define ADC_BASE ADCA_BASE

#endif /* ADC_ETCM_H_ */
