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

#define TIMER0_DEFAULT_PERIOD   1     // (mS)
#define NUM_SOFTWARE_TIMERS     3

extern uint16_t cpuTimer0IntCount;

typedef struct {
    volatile uint32_t count;
    volatile uint32_t period;
    volatile bool expired;
    volatile bool enabled;
}softwareTimers_t;

//softwareTimers_t softwareTimers[NUM_SOFTWARE_TIMERS];

typedef enum {
    TIMER0 = 0,
    TIMER1 = 1,
    TIMER2 = 2,
    TIMER3 = 3,
} softwareTimer_Numbers;

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
void initSoftwareTimers();
void updateSoftwareTimers();
void setSoftwareTimerPeriod(uint32_t period, softwareTimer_Numbers timerNumber);
bool checkSoftwareTimer(softwareTimer_Numbers timerNumber);
void resetSoftwareTimer(softwareTimer_Numbers timerNumber);

#endif /* TIMER_ETCM_H_ */
