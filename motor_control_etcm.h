/*
 * dac_etcm.h
 *
 *  Created on: Nov 4, 2019
 *      Author: Stephen Welch
 */

#ifndef MOTOR_CONTROL_ETCM_H_
#define MOTOR_CONTROL_ETCM_H_

#include "dac.h"

#define MOTOR_CONTROL_DAC_BASE_ADDRESS DACB_BASE
#define MOTOR_CONTROL_DAC_VREF 3.00
#define MOTOR_CONTROL_DAC_BITS 4096 - 1

// Temporary value, this should move to a dedicated header file
#define MOTOR_CONTROL_MAX_TORQUE 5.0

void initMotorControlDAC(void);
void setMotorControlDACOutputVoltage(float value);
void setMotorControlDACOutputRaw(uint16_t request);
void requestMotorControlTorque(float torque);

#endif /* MOTOR_CONTROL_ETCM_H_ */

