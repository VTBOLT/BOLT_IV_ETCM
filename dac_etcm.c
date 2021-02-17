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
void initDAC(uint32_t base)
{

    // This can be set to either the DAC's reference pin (DAC_REF_VDAC)
    // or the ADC's VREFHI pin (DAC_REF_ADC_VREFHI)
    DAC_setReferenceVoltage(base, DAC_REF_ADC_VREFHI);
    // Enable output
    DAC_enableOutput(base);
    DAC_tuneOffsetTrim(base, DAC_VREF);
    // Set the DAC shadow output to 0
    DAC_setShadowValue(base, 0);

    // Delay for buffered DAC to power up
    DEVICE_DELAY_US(10);


}

void setDACOutputRaw(uint32_t base, uint16_t request) {
    DAC_setShadowValue(base, request);
    DEVICE_DELAY_US(2);
}


void setDACOutputVoltage(uint32_t base, float voltage) {
    // output_voltage = (request * ref_voltage) / 4096
    float request = (voltage * 4096) / DAC_VREF;
    setDACOutputRaw(base, request);
}

//send torque request to motor controller
//void requestTorque(float torque)
//{
//    // Scale torque -> raw DAC request
//    // Not scaling to voltage saves a few FP operations
//    uint16_t request = (torque / MAX_TORQUE) * DAC_BITS;
//    setDACOutputRaw(request);
//}
