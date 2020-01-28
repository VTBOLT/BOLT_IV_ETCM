/*
 * timer_etcm.h
 *
 *  Created on: Jan 21, 2020
 *      Author: Tyler
 */

#ifndef TIMER_ETCM_H_
#define TIMER_ETCM_H_

//
// Included Files
//
#include "driverlib.h"
#include "device.h"

#define TIMER0_DEFAULT_PERIOD   1000     // 1 SECOND (mS)

extern uint16_t cpuTimer0IntCount;

//
// Function Prototypes
//
__interrupt void cpuTimer0ISR(void);
void initTimer0(void);
//void initCPUTimers(void);
void configCPUTimer(uint32_t cpuTimer, float period);
void reloadTimer0(float period);
void startTimer0(void);
void stopTimer0(void);
void initTimer0Interrupt(void);

#endif /* TIMER_ETCM_H_ */
