/*
 * timer.c
 *
 *  Created on: Jan 21, 2020
 *      Author: Tyler
 *
 * Function call order:
 * initTimer0()
 * initTimer0Interrupt() (see note in function header)
 *
 */


#include <timer_etcm.h>
#include <leds_etcm.h>

//
// Globals
//
uint16_t cpuTimer0IntCount;

/**
 * If timer does not seem to expire at expected interval, check
 * the clock pre-scaler value.
 *
 * TODO: automatic calculation of pre-scale value based
 * on desired period.
 */
void initTimer0(void){
    // Initialize timer period to maximum
    //
    CPUTimer_setPeriod(CPUTIMER0_BASE, 0xFFFFFFFF);

    // Initialize pre-scale counter to divide by 1 (SYSCLKOUT)
    //
    CPUTimer_setPreScaler(CPUTIMER0_BASE, 0);

    // Make sure timer is stopped
    //
    CPUTimer_stopTimer(CPUTIMER0_BASE);

    //
    // Reload all counter register with period value
    //
    CPUTimer_reloadTimerCounter(CPUTIMER0_BASE);

    // Reset interrupt counter
    //
    cpuTimer0IntCount = 0;

    // Load timer with default value
    configCPUTimer(CPUTIMER0_BASE, TIMER0_DEFAULT_PERIOD);
}

/**
 * initTimer0Interrupt enables the PIE CPU interrupt for Timer0 and
 * registers the ISR.
 *
 * Note: This function must be preceded by calls to Interrupt_initModule()
 * and Interrupt_initVectorTable() to ensure that the vector table is
 * set-up correctly.
 */
void initTimer0Interrupt(void){
    // register Timer0 ISR
    Interrupt_register(INT_TIMER0, &cpuTimer0ISR);

    CPUTimer_clearOverflowFlag(CPUTIMER0_BASE);
    //
    // To ensure precise timing, use write-only instructions to write to the
    // entire register. Therefore, if any of the configuration bits are changed
    // in configCPUTimer and initCPUTimers, the below settings must also
    // be updated.
    //
    CPUTimer_enableInterrupt(CPUTIMER0_BASE);
    Interrupt_enable(INT_TIMER0);
}

void startTimer0(void){
    CPUTimer_clearOverflowFlag(CPUTIMER0_BASE);
    CPUTimer_startTimer(CPUTIMER0_BASE);

}

void stopTimer0(void){
    Interrupt_disable(INT_TIMER0);
    CPUTimer_stopTimer(CPUTIMER0_BASE);
}

/**
 * reloadTimer0(float period) reloads the timer with a user specified
 * The application must restart the timer.
 *
 * @param period Period value in mili-seconds
 */
void reloadTimer0(float period){
    configCPUTimer(CPUTIMER0_BASE, period);
}

//
// configCPUTimer - This function initializes the selected timer to the
// period specified by the "freq" and "period" parameters. The "freq" is
// entered as Hz and the period in mSeconds. The timer is held in the stopped
// state after configuration.
//
void configCPUTimer(uint32_t cpuTimer, float period)
{
    uint32_t temp;

    //
    // Initialize timer period:
    //
    temp = (uint32_t)(DEVICE_SYSCLK_FREQ / 1000 * period);
    CPUTimer_setPeriod(cpuTimer, temp);

    //
    // Set pre-scale counter to divide by 1 (SYSCLKOUT):
    //
    CPUTimer_setPreScaler(cpuTimer, 0);

    //
    // Initializes timer control register. The timer is stopped, reloaded,
    // free run disabled, and interrupt enabled.
    // Additionally, the free and soft bits are set
    //
    CPUTimer_stopTimer(cpuTimer);
    CPUTimer_reloadTimerCounter(cpuTimer);
    CPUTimer_setEmulationMode(cpuTimer, CPUTIMER_EMULATIONMODE_STOPAFTERNEXTDECREMENT);
    CPUTimer_enableInterrupt(cpuTimer);

    //
    // Resets interrupt counters for the three cpuTimers
    //
    if (cpuTimer == CPUTIMER0_BASE)
    {
        cpuTimer0IntCount = 0;
    }
//    else if(cpuTimer == CPUTIMER1_BASE)
//    {
//        cpuTimer1IntCount = 0;
//    }
//    else if(cpuTimer == CPUTIMER2_BASE)
//    {
//        cpuTimer2IntCount = 0;
//    }
}

//
// cpuTimer0ISR - Counter for CpuTimer0
//
__interrupt void cpuTimer0ISR(void)
{
    cpuTimer0IntCount++;
    toggleRedLED();

    //
    // Acknowledge this interrupt to receive more interrupts from group 1
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}
