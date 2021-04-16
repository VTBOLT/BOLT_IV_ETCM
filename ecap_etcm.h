/*
 * capture_compare.h
 *
 *  Created on: Feb 25, 2020
 *      Author: Erin Freck
 */

#ifndef ECAP_ETCM_H_
#define ECAP_ETCM_H_

#include "ecap.h"
#include "device.h"

void runSpeedSensorECAP(void);
void initSpeedSensorECAP(void);
void initSpeedSensorEPWM(void);
void initSpeedSensorGPIO(void);
void initSpeedSensorInterrupts(void);
void testSpeedSensorECAP(void);
__interrupt void ecap1ISR(void);
uint16_t getRPMFront();
uint16_t getRPMRear();
uint16_t getFrequencyFront();
uint16_t getFrequencyRear();

#endif /* ECAP_ETCM_H_ */
