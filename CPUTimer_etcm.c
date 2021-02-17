/*
 * CPUTimer_etcm.c
 *
 *  Created on: Nov 4, 2019
 *      Author: Brady Siegel
 */

#include "CPUTimer_etcm.h"
void initTimer(uint32_t base, uint32_t period, uint16_t prescalar)
{
    CPUTimer_setPeriod(base, period);
    CPUTimer_setPreScaler(base, prescalar);
    CPUTimer_stopTimer(base);
    CPUTimer_reloadTimerCounter(base);
    CPUTimer_setEmulationMode(base, CPUTIMER_EMULATIONMODE_STOPAFTERNEXTDECREMENT);
    CPUTimer_enableInterrupt(base);
}
void startTimer(uint32_t base)
{
    CPUTimer_startTimer(base);
}
void stopTimer(uint32_t base)
{
    CPUTimer_stopTimer(base);
}
