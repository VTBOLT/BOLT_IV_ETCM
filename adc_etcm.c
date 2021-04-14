/*
 * adc_etcm.c
 *
 *  Created on: Nov 1, 2019
 *      Author: willc
 */

#include "adc_etcm.h"

//Initialize and power up the ADC
void initThrottleADC(void)
{
    //Set ADC divider to /4.
    ADC_setPrescaler(THROTTLE_ADC_BASE_ADDRESS, ADC_CLK_DIV_4_0);

    //Set resolution and signal mode and load the right trims.
    ADC_setMode(THROTTLE_ADC_BASE_ADDRESS, ADC_RESOLUTION_12BIT, ADC_MODE_SINGLE_ENDED);

    //Set pulse position to late.
    ADC_setInterruptPulseMode(THROTTLE_ADC_BASE_ADDRESS, ADC_PULSE_END_OF_CONV);

    //Power up ADC and delay for one ms.
    ADC_enableConverter(THROTTLE_ADC_BASE_ADDRESS);
    DEVICE_DELAY_US(1000);
}

//Initialize ADC SOC
void initThrottleADCSOC(void)
{
    ADC_setupSOC(THROTTLE_ADC_BASE_ADDRESS, THROTTLE_ADC_SOC, ADC_TRIGGER_SW_ONLY,
                         THROTTLE_ADC_CHANNEL, 15);

    ADC_setInterruptSource(THROTTLE_ADC_BASE_ADDRESS, THROTTLE_ADC_INT_NUM, THROTTLE_ADC_SOC);
    ADC_enableInterrupt(THROTTLE_ADC_BASE_ADDRESS, THROTTLE_ADC_INT_NUM);
    ADC_clearInterruptStatus(THROTTLE_ADC_BASE_ADDRESS, THROTTLE_ADC_INT_NUM);
}

uint16_t getThrottleADCRawValue() {
    // Force read
    ADC_forceSOC(THROTTLE_ADC_BASE_ADDRESS, THROTTLE_ADC_SOC);
    // Block until interrupt triggers to signal read completion
    while(ADC_getInterruptStatus(THROTTLE_ADC_BASE_ADDRESS, THROTTLE_ADC_INT_NUM) == false)
    {
    }
    // Clear the interrupt immediately
    ADC_clearInterruptStatus(THROTTLE_ADC_BASE_ADDRESS, THROTTLE_ADC_INT_NUM);

    // Return ADC result
    uint16_t result = ADC_readResult(THROTTLE_ADC_RESULT_BASE_ADDRESS, THROTTLE_ADC_SOC);
    return result;
}

double getThrottleVoltage(void) {
    float voltage = getThrottleADCRawValue() / 4096.0 * 3.0;
    return voltage;
}

