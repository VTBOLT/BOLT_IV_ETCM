/*
 * interrupt_etcm.c
 *
 *  Created on: Nov 5, 2019
 *      Author: Brady Siegel
 */

#include "interrupt_etcm.h"

void initInterrupt()
{
    Interrupt_initModule();
    Interrupt_initVectorTable();
}
void addInterrupt(void (*handler)(void), uint32_t interruptNum)
{
    Interrupt_register(interruptNum, handler);
}
void enableInterrupts()
{
    Interrupt_enable(INT_TIMER0);
}


