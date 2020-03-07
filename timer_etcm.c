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
#include <watchdog_etcm.h>

//
// Globals
//
uint16_t cpuTimer0IntCount;

softwareTimers_t softwareTimers[NUM_SOFTWARE_TIMERS];



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

    // Init software timers
    initSoftwareTimers();
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

/**
 * Interrupt handler for Timer0.
 * Called every period.
 *
 * TODO: Implement software timers
 */
__interrupt void cpuTimer0ISR(void)
{
    // prevent other interrupt jumps
    Interrupt_disableMaster();

    updateSoftwareTimers();

    //cpuTimer0IntCount++;
    //toggleStatusLED();
    //resetWatchdog();

    // send CAN data
    //sendCAN();



    // Acknowledge this interrupt to receive more interrupts from group 1
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);

    Interrupt_enableMaster();
}

void initSoftwareTimers(){
    int i;
    for (i = 0; i < NUM_SOFTWARE_TIMERS; i++){
        softwareTimers[i].count = 0;
        softwareTimers[i].period = TIMER0_DEFAULT_PERIOD;
        softwareTimers[i].expired = false;
        softwareTimers[i].enabled = false;
    }
}

void updateSoftwareTimers(){
    int i;
    for (i = 0; i < NUM_SOFTWARE_TIMERS; i++){
        if (softwareTimers[i].enabled && !softwareTimers[i].expired){
            softwareTimers[i].count += 1;
        }
    }
}

bool checkSoftwareTimer(softwareTimer_Numbers timerNumber){
    // error check
    if ((timerNumber >= NUM_SOFTWARE_TIMERS) || (!softwareTimers[timerNumber].enabled)){
        return false;
    }

    if (softwareTimers[timerNumber].count >= softwareTimers[timerNumber].period){
        softwareTimers[timerNumber].expired = true;

    }
    // check for period elapse
    return softwareTimers[timerNumber].expired;
}

void resetSoftwareTimer(softwareTimer_Numbers timerNumber){
    // error check
    if ((timerNumber >= NUM_SOFTWARE_TIMERS) || (!softwareTimers[timerNumber].enabled)){
        return;
    }
    softwareTimers[timerNumber].count = 0;
    softwareTimers[timerNumber].expired = false;
}

void setSoftwareTimerPeriod(uint32_t period, softwareTimer_Numbers timerNumber){
    // error check
    if (timerNumber >= NUM_SOFTWARE_TIMERS){
        return;
    }

    // disable and clear timer
    softwareTimers[timerNumber].enabled = false;
    softwareTimers[timerNumber].count = 0;

    // set period
    softwareTimers[timerNumber].period = period;

    // enable timer
    softwareTimers[timerNumber].enabled = true;

}
