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

#define THROTTLE_ADC_BASE_ADDRESS ADCB_BASE
#define THROTTLE_ADC_RESULT_BASE_ADDRESS ADCBRESULT_BASE
#define THROTTLE_ADC_CHANNEL ADC_CH_ADCIN4
#define THROTTLE_ADC_SOC ADC_SOC_NUMBER0
#define THROTTLE_ADC_INT_NUM ADC_INT_NUMBER2

void initThrottleADC(void);
//void initEPWM(void);
void initThrottleADCSOC(void);
uint16_t getThrottleADCRawValue(void);
double getThrottleVoltage(void);
//__interrupt void adcA0ISR(void);


#endif /* ADC_ETCM_H_ */
