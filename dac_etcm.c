/*
 * dac_etcm.c
 *
 *  Created on: Nov 7, 2019
 *      Author: cjosh
 */
#include "dac_etcm.h"

/*
 * Initializes the DAC. This is a blocking call because we have to wait
 * 1 ms for the DAC to power up.
 */
void initDAC(void)
{

    // This can be set to either the DAC's reference pin (DAC_REF_VDAC)
    // or the ADC's VREFHI pin (DAC_REF_ADC_VREFHI)
    DAC_setReferenceVoltage(DACA_BASE, DAC_REF_VDAC);

    // Enable output
    DAC_enableOutput(DACA_BASE);

    // Set the DAC shadow output to 0
    DAC_setShadowValue(DACA_BASE, 0);

    // Delay for buffered DAC to power up
    DEVICE_DELAY_US(10);
}

void setDACVoltage(uint16_t voltage) {
    // DAC_output = (output_voltage * reference_voltage) / 4096
//    DAC_setShadowValue(DACA_BASE, (voltage * 3.3) / 4096);
    DAC_setShadowValue(DACA_BASE, 2048);
}


//send torque request to motor controller
void requestTorque(int torque)
{

}
