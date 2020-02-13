/*
 * adc_etcm.c
 *
 *  Created on: Nov 1, 2019
 *      Author: willc
 *      Contributer: Tyler Shaffer
 */

#include "adc_etcm.h"


//Initialize and power up the ADC
void initADC(void)
{
    //Set ADC divider to /4.
    ADC_setPrescaler(ADCA_BASE, ADC_CLK_DIV_4_0);

    //Set resolution and signal mode and load the right trims.
    ADC_setMode(ADCA_BASE, ADC_RESOLUTION_12BIT, ADC_MODE_SINGLE_ENDED);

    //Set pulse position to late.
    ADC_setInterruptPulseMode(ADCA_BASE, ADC_PULSE_END_OF_CONV);

    //Power up ADC and delay for one ms.
    ADC_enableConverter(ADCA_BASE);
    DEVICE_DELAY_US(1000);

    // init SOCs
    initADCSOC();
}

//Initialize EPWM
void initEPWM(void)
{

}

/**
 * Map SOCs to ADC input channels
 *
 * ADC_B
 * ADCIN5 -> Front suspension travel
 * ADCIN4 -> Rear suspension travel
 *
 * ADC_A
 * ADCIN4 -> Handgrip throttle (pin 69)
 */
void initADCSOC(void)
{
    /*
     * Throttle SOC
     */
    ADC_setupSOC(ADCA_BASE, ADC_SOC_THROTTLE, ADC_TRIGGER_SW_ONLY, ADC_CH_THROTTLE, ADC_SAMPLE_TIME_CYCLES);
    // enable interrupt flag
    ADC_setInterruptSource(ADCA_BASE, ADC_INT_THROTTLE, ADC_SOC_THROTTLE);
    ADC_enableInterrupt(ADCA_BASE, ADC_INT_THROTTLE);
    ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_THROTTLE);

}

/**
 * Returns the ADC value of the throttle as an unsigned 16-bit integer.
 *
 * Reference Voltage: 3V0
 *
 * For demo purpose only. This wastes some CPU time.
 */
uint16_t getThrottleADC(void){
    // Force a SOC
    ADC_forceSOC(ADCA_BASE, ADC_SOC_THROTTLE);
    // Wait for conversion completion
    while(!ADC_getInterruptStatus(ADCA_BASE, ADC_INT_THROTTLE));
    // clear INT flag
    ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_THROTTLE);

    // return results
    return ADC_readResult(ADCARESULT_BASE, ADC_SOC_THROTTLE);
}

#define ADC_RESOLUTION_BITS 4095
#define ADC_REF_VOLTAGE     3

float getThrottlePercent(uint16_t throttleADCraw){
    // grab throttle ADC value
    //uint16_t throttleADCraw = getThrottleADC();

    // find percentage
    uint16_t throttlePercent = (throttleADCraw / (float)ADC_RESOLUTION_BITS) * 100;

    // error check and bound

    // return with value
    return throttlePercent;
}
