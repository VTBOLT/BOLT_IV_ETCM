/*
 * CPUTimer_etcm.h
 *
 *  Created on: Nov 4, 2019
 *      Author: Brady Siegel
 */

#ifndef TIMER_H_
#define TIMER_H_

#include "driverlib.h"
#include "device.h"
#include "cputimer.h"

void initTimer(uint32_t base, uint32_t period, uint16_t prescalar);
void startTimer(uint32_t base);
void stopTimer(uint32_t base);

#endif /* CPUTIMER_ETCM_H_ */
