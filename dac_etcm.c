/*
 * dac_etcm.c
 *
 *  Created on: Nov 7, 2019
 *      Author: cjosh
 */

#include "dac_etcm.h"
#include "device.h"

/*
 * Initializes the DAC. This is a blocking call because we have to wait
 * 1 ms for the DAC to power up.
 */
void initDAC(void)
{

    // This can be set to either the DAC's reference pin (DAC_REF_VDAC)
    // or the ADC's VREFHI pin (DAC_REF_ADC_VREFHI)
    DAC_setReferenceVoltage(DAC_BASE_ADDRESS, DAC_REF_ADC_VREFHI);
    // Enable output
    DAC_enableOutput(DAC_BASE_ADDRESS);
    DAC_tuneOffsetTrim(DAC_BASE_ADDRESS, DAC_VREF);
    // Set the DAC shadow output to 0
    DAC_setShadowValue(DAC_BASE_ADDRESS, 0);

    // Delay for buffered DAC to power up
    DEVICE_DELAY_US(10);
}

void setDACOutputRaw(uint16_t request) {
    DAC_setShadowValue(DAC_BASE_ADDRESS, request);
    DEVICE_DELAY_US(2);
}


void setDACOutputVoltage(float voltage) {
    // output_voltage = (request * ref_voltage) / 4096
    uint16_t request = (voltage * 4095.0) / DAC_VREF;
    setDACOutputRaw(request);
}

//send torque request to motor controller
void requestTorque(float torque)
{
    /*
    // Scale torque -> raw DAC request
    // Not scaling to voltage saves a few FP operations
    uint16_t request = (torque / MAX_TORQUE) * DAC_BITS;
    setDACOutputRaw(request);
    */

    // Original implementation unneeded - torque request is 0-5V
    setDACOutputVoltage(torque);
}
