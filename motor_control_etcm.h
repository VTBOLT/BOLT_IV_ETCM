/*
 * dac_etcm.h
 *
 *  Created on: Nov 4, 2019
 *      Author: Stephen Welch
 */

#ifndef MOTOR_CONTROL_ETCM_H_
#define MOTOR_CONTROL_ETCM_H_

#include "dac.h"

#define DAC_BASE_ADDRESS DACB_BASE
#define DAC_VREF 3.00
#define DAC_BITS 4096 - 1

// Temporary value, this should move to a dedicated header file
#define MAX_TORQUE 5.0

void initDAC(void);
void setDACOutputVoltage(float value);
void setDACOutputRaw(uint16_t request);
void requestTorque(float torque);

#endif /* MOTOR_CONTROL_ETCM_H_ */

