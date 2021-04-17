/*
 * dac_etcm.c
 *
 *  Created on: Nov 7, 2019
 *      Author: cjosh
 */

#include <motor_control_etcm.h>
#include "device.h"

/*
 * Initializes the DAC. This is a blocking call because we have to wait
 * 1 ms for the DAC to power up.
 */
void initMotorControlDAC(void)
{

    // This can be set to either the DAC's reference pin (DAC_REF_VDAC)
    // or the ADC's VREFHI pin (DAC_REF_ADC_VREFHI)
    DAC_setReferenceVoltage(MOTOR_CONTROL_DAC_BASE_ADDRESS, DAC_REF_ADC_VREFHI);
    // Enable output
    DAC_enableOutput(MOTOR_CONTROL_DAC_BASE_ADDRESS);
    DAC_tuneOffsetTrim(MOTOR_CONTROL_DAC_BASE_ADDRESS, MOTOR_CONTROL_DAC_VREF);
    // Set the DAC shadow output to 0
    DAC_setShadowValue(MOTOR_CONTROL_DAC_BASE_ADDRESS, 0);

    // Delay for buffered DAC to power up
    DEVICE_DELAY_US(10);
}

void setMotorControlDACOutputRaw(uint16_t request) {
    DAC_setShadowValue(MOTOR_CONTROL_DAC_BASE_ADDRESS, request);
    DEVICE_DELAY_US(2);
}


void setMotorControlDACOutputVoltage(float voltage) {
    // output_voltage = (request * ref_voltage) / 4096
    uint16_t request = (voltage * 4095.0) / MOTOR_CONTROL_DAC_VREF;
    setMotorControlDACOutputRaw(request);
}

//send torque request to motor controller
void requestMotorControlTorque(float torque)
{
    /*
    // Scale torque -> raw DAC request
    // Not scaling to voltage saves a few FP operations
    uint16_t request = (torque / MAX_TORQUE) * DAC_BITS;
    setDACOutputRaw(request);
    */

    // Original implementation unneeded - torque request is 0-5V
    setMotorControlDACOutputVoltage(torque);
}
