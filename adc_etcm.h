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

#define ADC_SOC_THROTTLE    ADC_SOC_NUMBER0
#define ADC_CH_THROTTLE     ADC_CH_ADCIN4
#define ADC_INT_THROTTLE    ADC_INT_NUMBER1

/////////////////////////////////////////////////////////////////////
// See page 1587 of tech ref. manual for more detail
// Allows sample capacitor time to settle
// Using 12-bit resolution
#define ADC_SAMPLE_TIME 270e-9
#define ADC_SAMPLE_TIME_CYCLES (ADC_SAMPLE_TIME * DEVICE_SYSCLK_FREQ)
/////////////////////////////////////////////////////////////////////

void initADC(void);
void initEPWM(void);
void initADCSOC(void);
__interrupt void adcA0ISR(void);
uint16_t getThrottleADC(void);


#endif /* ADC_ETCM_H_ */
