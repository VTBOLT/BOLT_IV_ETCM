//#############################################################################
//
// FILE:    capture_compare_main.c
//
// TITLE:   Capture_Compare.
// Author:  Erin Freck
// Verion: February 21, 2020
//
//
// This program finds rpm by calculating frequency of a PWM wave.
//
// Rear Wheel is on GPIO1
// Front Wheel is on GPIO2
//
//  Motor RPM Range: 0-8000
//  Wheel RPM Range: 0-459
//  Frequency Range: 0-18333
//
//#############################################################################

//
// Included Files
//
#include "driverlib.h"
#include "device.h"

//
// Defines
//
#define PWM3_TIMER_MIN     500U
#define PWM3_TIMER_MAX     8000U
#define EPWM_TIMER_UP      1U
#define EPWM_TIMER_DOWN    0U
#define ENCODER_DISK_HOLES 40;

//
// Globals
//
uint32_t epwm3TimerDirection;
volatile uint32_t cap2Count_rear;
volatile uint32_t cap3Count_rear;
volatile uint32_t cap4Count_rear;
volatile uint32_t cap2Count_front;
volatile uint32_t cap3Count_front;
volatile uint32_t cap4Count_front;

volatile float eCAPsignalFreq_rear;
volatile float eCAPsignalPeriod_rear;
volatile float rpm_rear;
volatile float eCAPsignalFreq_front;
volatile float eCAPsignalPeriod_front;
volatile float rpm_front;

//
// Function Prototypes
//
void initECAP(void);
void initEPWM(void);
void testECAP(void);
__interrupt void ecap1ISR(void);
uint32_t getRPMRear();
uint32_t getFrequencyRear();
uint32_t getRPMFront();
uint32_t getFrequencyFront();

//
// Main
//
void runECAP(void)
{
    //
    // Initialize device clock and peripherals
    //
    Device_init();

    //
    // Disable pin locks and enable internal pullups.
    //
    Device_initGPIO();

    //
    // Initialize PIE and clear PIE registers. Disables CPU interrupts.
    //
    Interrupt_initModule();

    //
    // Initialize the PIE vector table with pointers to the shell Interrupt
    // Service Routines (ISR).
    //
    Interrupt_initVectorTable();

    //
    // Configure GPIO 1 as rear eCAP input
    //
    XBAR_setInputPin(XBAR_INPUT7, 1);
    GPIO_setPinConfig(GPIO_1_GPIO1);
    GPIO_setDirectionMode(1, GPIO_DIR_MODE_IN);
    GPIO_setQualificationMode(1, GPIO_QUAL_ASYNC);

    //
    // Configure GPIO 2 as front eCAP input
    //
    XBAR_setInputPin(XBAR_INPUT8, 2);
    GPIO_setPinConfig(GPIO_2_GPIO2);
    GPIO_setDirectionMode(2, GPIO_DIR_MODE_IN);
    GPIO_setQualificationMode(2, GPIO_QUAL_ASYNC);

    //
    // Interrupts that are used in this example are re-mapped to ISR functions
    // found within this file.
    //
    Interrupt_register(INT_ECAP1, &ecap1ISR);
    Interrupt_register(INT_ECAP2, &ecap1ISR);

    //
    // Configure ePWM and eCAP
    //
    initEPWM();
    initECAP();

    //
    // Initialize counters:
    //
    cap2Count_rear = 0U;
    cap3Count_rear = 0U;
    cap4Count_rear = 0U;
    cap2Count_front = 0U;
    cap3Count_front = 0U;
    cap4Count_front = 0U;

    //
    // Enable interrupts required for this example
    //
    Interrupt_enable(INT_ECAP1);
    Interrupt_enable(INT_ECAP2);

    //
    // Enable Global Interrupt (INTM) and Real time interrupt (DBGM)
    //
    EINT;
    ERTM;

    //
    // Loop forever. Suspend or place breakpoints to observe the buffers.
    //
    for(;;)
    {
        NOP;
    }
}

//
// initEPWM - Configure ePWM
//
void initEPWM()
{
    //
    // Disable sync(Freeze clock to PWM as well)
    //
    SysCtl_disablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);

    //
    // Configure ePWM
    //       Counter runs in up-count mode.
    //       Action qualifier will toggle output on period match
    //
    EPWM_setTimeBaseCounterMode(EPWM3_BASE, EPWM_COUNTER_MODE_UP);
    EPWM_setTimeBasePeriod(EPWM3_BASE, PWM3_TIMER_MIN);
    EPWM_setPhaseShift(EPWM3_BASE, 0U);
    EPWM_setActionQualifierAction(EPWM3_BASE,
                                  EPWM_AQ_OUTPUT_A,
                                  EPWM_AQ_OUTPUT_TOGGLE,
                                  EPWM_AQ_OUTPUT_ON_TIMEBASE_PERIOD);
    EPWM_setClockPrescaler(EPWM3_BASE,
                           EPWM_CLOCK_DIVIDER_1,
                           EPWM_HSCLOCK_DIVIDER_2);

    epwm3TimerDirection = EPWM_TIMER_UP;

    //
    // Enable sync and clock to PWM
    //
    SysCtl_enablePeripheral(SYSCTL_PERIPH_CLK_TBCLKSYNC);
}

//
// initECAP - Configure eCAP
//
void initECAP()
{
    //
    // Disable ,clear all capture flags and interrupts
    //
    ECAP_disableInterrupt(ECAP1_BASE,
                          (ECAP_ISR_SOURCE_CAPTURE_EVENT_1  |
                           ECAP_ISR_SOURCE_CAPTURE_EVENT_2  |
                           ECAP_ISR_SOURCE_CAPTURE_EVENT_3  |
                           ECAP_ISR_SOURCE_CAPTURE_EVENT_4  |
                           ECAP_ISR_SOURCE_COUNTER_OVERFLOW |
                           ECAP_ISR_SOURCE_COUNTER_PERIOD   |
                           ECAP_ISR_SOURCE_COUNTER_COMPARE));
    ECAP_clearInterrupt(ECAP1_BASE,
                        (ECAP_ISR_SOURCE_CAPTURE_EVENT_1  |
                         ECAP_ISR_SOURCE_CAPTURE_EVENT_2  |
                         ECAP_ISR_SOURCE_CAPTURE_EVENT_3  |
                         ECAP_ISR_SOURCE_CAPTURE_EVENT_4  |
                         ECAP_ISR_SOURCE_COUNTER_OVERFLOW |
                         ECAP_ISR_SOURCE_COUNTER_PERIOD   |
                         ECAP_ISR_SOURCE_COUNTER_COMPARE));

    ECAP_disableInterrupt(ECAP2_BASE,
                              (ECAP_ISR_SOURCE_CAPTURE_EVENT_1  |
                               ECAP_ISR_SOURCE_CAPTURE_EVENT_2  |
                               ECAP_ISR_SOURCE_CAPTURE_EVENT_3  |
                               ECAP_ISR_SOURCE_CAPTURE_EVENT_4  |
                               ECAP_ISR_SOURCE_COUNTER_OVERFLOW |
                               ECAP_ISR_SOURCE_COUNTER_PERIOD   |
                               ECAP_ISR_SOURCE_COUNTER_COMPARE));
    ECAP_clearInterrupt(ECAP2_BASE,
                            (ECAP_ISR_SOURCE_CAPTURE_EVENT_1  |
                             ECAP_ISR_SOURCE_CAPTURE_EVENT_2  |
                             ECAP_ISR_SOURCE_CAPTURE_EVENT_3  |
                             ECAP_ISR_SOURCE_CAPTURE_EVENT_4  |
                             ECAP_ISR_SOURCE_COUNTER_OVERFLOW |
                             ECAP_ISR_SOURCE_COUNTER_PERIOD   |
                             ECAP_ISR_SOURCE_COUNTER_COMPARE));

    //
    // Disable CAP1-CAP4 register loads
    //
    ECAP_disableTimeStampCapture(ECAP1_BASE);
    ECAP_disableTimeStampCapture(ECAP2_BASE);

    //
    // Configure eCAP
    //    Enable capture mode.
    //    One shot mode, stop capture at event 4.
    //    Set polarity of the events to rising, falling, rising, falling edge.
    //    Set capture in time difference mode.
    //    Select input from XBAR7.
    //    Enable eCAP module.
    //    Enable interrupt.
    //
    ECAP_stopCounter(ECAP1_BASE);
    ECAP_enableCaptureMode(ECAP1_BASE);
    ECAP_stopCounter(ECAP2_BASE);
    ECAP_enableCaptureMode(ECAP2_BASE);

    ECAP_setCaptureMode(ECAP1_BASE, ECAP_ONE_SHOT_CAPTURE_MODE, ECAP_EVENT_4);
    ECAP_setCaptureMode(ECAP2_BASE, ECAP_ONE_SHOT_CAPTURE_MODE, ECAP_EVENT_4);

    ECAP_setEventPolarity(ECAP1_BASE, ECAP_EVENT_1, ECAP_EVNT_FALLING_EDGE);
    ECAP_setEventPolarity(ECAP1_BASE, ECAP_EVENT_2, ECAP_EVNT_RISING_EDGE);
    ECAP_setEventPolarity(ECAP1_BASE, ECAP_EVENT_3, ECAP_EVNT_FALLING_EDGE);
    ECAP_setEventPolarity(ECAP1_BASE, ECAP_EVENT_4, ECAP_EVNT_RISING_EDGE);

    ECAP_setEventPolarity(ECAP2_BASE, ECAP_EVENT_1, ECAP_EVNT_FALLING_EDGE);
    ECAP_setEventPolarity(ECAP2_BASE, ECAP_EVENT_2, ECAP_EVNT_RISING_EDGE);
    ECAP_setEventPolarity(ECAP2_BASE, ECAP_EVENT_3, ECAP_EVNT_FALLING_EDGE);
    ECAP_setEventPolarity(ECAP2_BASE, ECAP_EVENT_4, ECAP_EVNT_RISING_EDGE);

    ECAP_enableCounterResetOnEvent(ECAP1_BASE, ECAP_EVENT_1);
    ECAP_enableCounterResetOnEvent(ECAP1_BASE, ECAP_EVENT_2);
    ECAP_enableCounterResetOnEvent(ECAP1_BASE, ECAP_EVENT_3);
    ECAP_enableCounterResetOnEvent(ECAP1_BASE, ECAP_EVENT_4);

    ECAP_enableCounterResetOnEvent(ECAP2_BASE, ECAP_EVENT_1);
    ECAP_enableCounterResetOnEvent(ECAP2_BASE, ECAP_EVENT_2);
    ECAP_enableCounterResetOnEvent(ECAP2_BASE, ECAP_EVENT_3);
    ECAP_enableCounterResetOnEvent(ECAP2_BASE, ECAP_EVENT_4);

    XBAR_setInputPin(XBAR_INPUT7, 1);
    XBAR_setInputPin(XBAR_INPUT8, 2);

    ECAP_enableLoadCounter(ECAP1_BASE);
    ECAP_setSyncOutMode(ECAP1_BASE, ECAP_SYNC_OUT_SYNCI);
    ECAP_startCounter(ECAP1_BASE);
    ECAP_enableTimeStampCapture(ECAP1_BASE);
    ECAP_reArm(ECAP1_BASE);

    ECAP_enableLoadCounter(ECAP2_BASE);
    ECAP_setSyncOutMode(ECAP2_BASE, ECAP_SYNC_OUT_SYNCI);
    ECAP_startCounter(ECAP2_BASE);
    ECAP_enableTimeStampCapture(ECAP2_BASE);
    ECAP_reArm(ECAP2_BASE);

    ECAP_enableInterrupt(ECAP1_BASE, ECAP_ISR_SOURCE_CAPTURE_EVENT_4);
    ECAP_enableInterrupt(ECAP2_BASE, ECAP_ISR_SOURCE_CAPTURE_EVENT_4);
}

//
// eCAP 1 ISR
//
__interrupt void ecap1ISR(void)
{
    //
    // Get the capture counts. Each capture should be 4x the ePWM count
    // because of the ePWM clock dividers.
    //
    cap2Count_rear = ECAP_getEventTimeStamp(ECAP1_BASE, ECAP_EVENT_2);
    cap3Count_rear = ECAP_getEventTimeStamp(ECAP1_BASE, ECAP_EVENT_3);
    cap4Count_rear = ECAP_getEventTimeStamp(ECAP1_BASE, ECAP_EVENT_4);

    cap2Count_front = ECAP_getEventTimeStamp(ECAP2_BASE, ECAP_EVENT_2);
    cap3Count_front = ECAP_getEventTimeStamp(ECAP2_BASE, ECAP_EVENT_3);
    cap4Count_front = ECAP_getEventTimeStamp(ECAP2_BASE, ECAP_EVENT_4);

    eCAPsignalPeriod_rear = cap2Count_rear / (float)DEVICE_SYSCLK_FREQ;
    eCAPsignalFreq_rear = (1.0 / eCAPsignalPeriod_rear) / 4;
    rpm_rear = eCAPsignalFreq_rear/ENCODER_DISK_HOLES;

    eCAPsignalPeriod_front = cap2Count_front / (float)DEVICE_SYSCLK_FREQ;
    eCAPsignalFreq_front = (1.0 / eCAPsignalPeriod_front) / 4;
    rpm_front = eCAPsignalFreq_front/ENCODER_DISK_HOLES;

    //
    // Clear interrupt flags for more interrupts.
    //
    ECAP_clearInterrupt(ECAP1_BASE,ECAP_ISR_SOURCE_CAPTURE_EVENT_4);
    ECAP_clearGlobalInterrupt(ECAP1_BASE);
    ECAP_clearInterrupt(ECAP2_BASE,ECAP_ISR_SOURCE_CAPTURE_EVENT_4);
    ECAP_clearGlobalInterrupt(ECAP2_BASE);

    //
    // Start eCAP
    //
    ECAP_reArm(ECAP1_BASE);
    ECAP_reArm(ECAP2_BASE);

    //
    // Acknowledge the group interrupt for more interrupts.
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP4);
}

uint32_t getRPMRear()
{
    return rpm_rear;
}

uint32_t getFrequencyRear()
{
    return eCAPsignalFreq_rear;
}

uint32_t getRPMFront()
{
    return rpm_front;
}

uint32_t getFrequencyFront()
{
    return eCAPsignalFreq_front;
}

void testECAP()
{
    runECAP();
}

//
// End File
//
