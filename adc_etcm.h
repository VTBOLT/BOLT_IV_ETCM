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

void initADC(void);
void initEPWM(void);
void initADCSOC(void);
__interrupt void adcA0ISR(void);


#endif /* ADC_ETCM_H_ */
