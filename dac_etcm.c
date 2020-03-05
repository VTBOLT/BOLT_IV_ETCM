/*
 * dac_etcm.c
 *
 *  Created on: Nov 7, 2019
 *
 */
#include "dac_etcm.h"



void initDAC(void)
{
    //
    // Set VREFHI (3V0) as the DAC reference voltage.
    //
    DAC_setReferenceVoltage(THROTTLE_DAC_BASE, DAC_REF_ADC_VREFHI);

    //
    // Enable the DAC output
    //
    DAC_enableOutput(THROTTLE_DAC_BASE);

    //
    // Set the DAC shadow output to 0
    //
    DAC_setShadowValue(THROTTLE_DAC_BASE, 0);

    //
    // Delay for buffered DAC to power up
    //
    DEVICE_DELAY_US(10);

}

void setThrottleDAC(uint16_t dacValue){
    DAC_setShadowValue(THROTTLE_DAC_BASE, dacValue);
    DEVICE_DELAY_US(2);
}


//send torque request to motor controller
void requestTorque(int torque)
{

}
