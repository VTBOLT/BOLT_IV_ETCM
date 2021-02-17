/*
 * interrupt_etcm.h
 *
 *  Created on: Nov 5, 2019
 *      Author: Brady Siegel
 */

#ifndef INTERRUPT_ETCM_H_
#define INTERRUPT_ETCM_H_

#include "driverlib.h"
#include "device.h"

void initInterrupt();
void addInterrupt(void (*handler)(void), uint32_t interruptNum);
void enableInterrupts();



#endif /* INTERRUPT_ETCM_H_ */
