/*
 * dac_etcm.h
 *
 *  Created on: Nov 4, 2019
 *      Author: Stephen Welch
 */

#ifndef DAC_ETCM_H_
#define DAC_ETCM_H_

#include "dac.h"

#define DAC_BASE_ADDRESS DACA_BASE
#define DAC_VREF 3.04
#define DAC_BITS 4096 - 1

// Temporary value, this should move to a dedicated header file
#define MAX_TORQUE 150.0

void initDAC(uint32_t base);
void setDACOutputVoltage(uint32_t base, float value);
void setDACOutputRaw(uint32_t base, uint16_t request);
void requestTorque(float torque);

#endif /* DAC_ETCM_H_ */
