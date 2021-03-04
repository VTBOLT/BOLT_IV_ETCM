/*
 * adc_etcm.c
 *
 *  Created on: Nov 1, 2019
 *      Author: willc
 */

#include "adc_etcm.h"

//Initialize and power up the ADC
void initADC(void)
{
    //Set ADC divider to /4.
    ADC_setPrescaler(ADC_BASE_ADDRESS, ADC_CLK_DIV_4_0);

    //Set resolution and signal mode and load the right trims.
    ADC_setMode(ADC_BASE_ADDRESS, ADC_RESOLUTION_12BIT, ADC_MODE_SINGLE_ENDED);

    //Set pulse position to late.
    ADC_setInterruptPulseMode(ADC_BASE_ADDRESS, ADC_PULSE_END_OF_CONV);

    //Power up ADC and delay for one ms.
    ADC_enableConverter(ADC_BASE_ADDRESS);
    DEVICE_DELAY_US(1000);
}

//Initialize ADC SOC
void initADCSOC(void)
{
    ADC_setupSOC(ADC_BASE_ADDRESS, ADC_SOC_NUMBER0, ADC_TRIGGER_SW_ONLY,
                         ADC_CHANNEL, 15);

    ADC_setInterruptSource(ADC_BASE_ADDRESS, ADC_INT_NUMBER1, ADC_SOC_NUMBER0);
    ADC_enableInterrupt(ADC_BASE_ADDRESS, ADC_INT_NUMBER1);
    ADC_clearInterruptStatus(ADC_BASE_ADDRESS, ADC_INT_NUMBER1);
}

uint16_t getADCRawValue() {
    // Force read
    ADC_forceSOC(ADC_BASE_ADDRESS, ADC_SOC_NUMBER0);
    // Block until interrupt triggers to signal read completion
    while(ADC_getInterruptStatus(ADC_BASE_ADDRESS, ADC_INT_NUMBER1) == false)
    {
    }
    // Clear the interrupt immediately
    ADC_clearInterruptStatus(ADC_BASE_ADDRESS, ADC_INT_NUMBER1);

    // Return ADC result
    uint16_t result = ADC_readResult(ADC_RESULT_BASE_ADDRESS, ADC_SOC_NUMBER0);
    return result;
}

double getThrottleVoltage(void) {
    float voltage = getADCRawValue() / 4096.0 * 3.0;
    return voltage;
}

