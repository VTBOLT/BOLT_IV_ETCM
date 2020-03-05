/*
 * dac_etcm.h
 *
 *  Created on: Nov 7, 2019
 *      Author: cjosh
 */

#ifndef DAC_ETCM_H_
#define DAC_ETCM_H_

#include "device.h"

#define THROTTLE_DAC_BASE   DACB_BASE


void initDAC(void);
void requestTorque(int torque);
void setThrottleDAC(uint16_t dacValue);


#endif /* DAC_ETCM_H_ */
