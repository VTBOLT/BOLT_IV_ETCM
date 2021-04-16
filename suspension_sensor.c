#include "suspension_sensor.h"

uint16_t frontSensorSample;
volatile int16_t frontSensorPosition;
volatile bool frontWheelie;

uint16_t rearSensorSample;
volatile int16_t rearSensorPosition;
volatile bool rearWheelie;

//Initialize and power up the ADC
void initFrontSuspensionSensorADC(void)
{
    //Set ADC divider to /4.
    ADC_setPrescaler(FRONT_SUSP_SENSOR_ADC_BASE, ADC_CLK_DIV_4_0);

    //Set resolution and signal mode and load the right trims.
    ADC_setMode(FRONT_SUSP_SENSOR_ADC_BASE, ADC_RESOLUTION_12BIT, ADC_MODE_SINGLE_ENDED);

    //Set pulse position to late.
    ADC_setInterruptPulseMode(FRONT_SUSP_SENSOR_ADC_BASE, ADC_PULSE_END_OF_CONV);

    //Power up ADC and delay for one ms.
    ADC_enableConverter(FRONT_SUSP_SENSOR_ADC_BASE);
    DEVICE_DELAY_US(1000);
}

//Initialize EPWM
void initFrontSuspensionSensorEPWM(void)
{
    //Disable SOCA
    EPWM_disableADCTrigger(FRONT_SUSP_SENSOR_EPWM_BASE, FRONT_SUSP_SENSOR_EPWM_SOC);

    //Configure SOC to happen on up-count event.
    EPWM_setADCTriggerSource(FRONT_SUSP_SENSOR_EPWM_BASE, FRONT_SUSP_SENSOR_EPWM_SOC, EPWM_SOC_TBCTR_U_CMPA);
    EPWM_setADCTriggerEventPrescale(FRONT_SUSP_SENSOR_EPWM_BASE, FRONT_SUSP_SENSOR_EPWM_SOC, 1);

    //Set compare A value to 2048 and period to 4096.
    EPWM_setCounterCompareValue(FRONT_SUSP_SENSOR_EPWM_BASE, EPWM_COUNTER_COMPARE_A, 0x0800);
    EPWM_setTimeBasePeriod(FRONT_SUSP_SENSOR_EPWM_BASE, 0x1000);

    //Freeze the counter
    EPWM_setTimeBaseCounterMode(FRONT_SUSP_SENSOR_EPWM_BASE, EPWM_COUNTER_MODE_STOP_FREEZE);
}

//Initialize ADC SOC and configure the ADCA's SOC0 to be triggered by the ePWM1
void initFrontSuspensionSensorADCSOC(void)
{
    //Configure the SOC. The position sensor is connected to B4
    ADC_setupSOC(FRONT_SUSP_SENSOR_ADC_BASE, FRONT_SUSP_SENSOR_ADC_SOC, ADC_TRIGGER_SW_ONLY,
                 FRONT_SUSP_SENSOR_ADC_CHANNEL, 270e-9 * DEVICE_SYSCLK_FREQ);

    //Set the SOC0 to set interrupt 1 flag. Enable interrupt and clear flag.
    ADC_setInterruptSource(FRONT_SUSP_SENSOR_ADC_BASE, FRONT_SUSP_SENSOR_ADC_INT_NUM, FRONT_SUSP_SENSOR_ADC_SOC);
    ADC_enableInterrupt(FRONT_SUSP_SENSOR_ADC_BASE, FRONT_SUSP_SENSOR_ADC_INT_NUM);
    ADC_clearInterruptStatus(FRONT_SUSP_SENSOR_ADC_BASE, FRONT_SUSP_SENSOR_ADC_INT_NUM);
}

//Create the interrupt for ADC B Interrupt 1
__interrupt void frontSuspensionISR(void)
{
    //Read the raw result
    frontSensorSample = ADC_readResult(ADCBRESULT_BASE, FRONT_SUSP_SENSOR_ADC_SOC);

    //27.3 is the number of adc units in 1 mm of suspension travel.
    frontSensorPosition = (150 - frontSensorSample) / 27.3;

    //Check if the sensor is completely extended.
    frontWheelie = (frontSensorPosition == 150);

    //Clear interrupt flag
    ADC_clearInterruptStatus(FRONT_SUSP_SENSOR_ADC_BASE, FRONT_SUSP_SENSOR_ADC_INT_NUM);

    //Check for overflow.
    if (true == ADC_getInterruptOverflowStatus(FRONT_SUSP_SENSOR_ADC_BASE, FRONT_SUSP_SENSOR_ADC_INT_NUM))
    {
        ADC_clearInterruptOverflowStatus(FRONT_SUSP_SENSOR_ADC_BASE, FRONT_SUSP_SENSOR_ADC_INT_NUM);
        ADC_clearInterruptStatus(FRONT_SUSP_SENSOR_ADC_BASE, FRONT_SUSP_SENSOR_ADC_INT_NUM);
    }

    //Acknowledge the interrupt
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}

uint16_t getFrontSensorPosition(void)
{
    return frontSensorPosition;
}

bool getFrontWheelieStatus(void)
{
    return frontWheelie;
}

void initFrontSuspensionSensor(void)
{
    //Re-map interrupts in this file to ISR functions in this file.
    Interrupt_register(FRONT_SUSP_SENSOR_ADC_INT, &frontSuspensionISR);

    //Set up the ADC and ePWM and initialize the SOC
    initFrontSuspensionSensorADC();
    initFrontSuspensionSensorEPWM();
    initFrontSuspensionSensorADCSOC();

    //Enable ADC interrupt
    Interrupt_enable(FRONT_SUSP_SENSOR_ADC_INT);

    //Start ePWM1, enabling SCB and put counter in up-count
    EPWM_enableADCTrigger(FRONT_SUSP_SENSOR_EPWM_BASE, FRONT_SUSP_SENSOR_EPWM_SOC);
    EPWM_setTimeBaseCounterMode(FRONT_SUSP_SENSOR_EPWM_BASE, EPWM_COUNTER_MODE_UP);

}

//Initialize and power up the ADC
void initRearSuspensionSensorADC(void)
{
    //Set ADC divider to /4.
    ADC_setPrescaler(REAR_SUSP_SENSOR_ADC_BASE, ADC_CLK_DIV_4_0);

    //Set resolution and signal mode and load the right trims.
    ADC_setMode(REAR_SUSP_SENSOR_ADC_BASE, ADC_RESOLUTION_12BIT, ADC_MODE_SINGLE_ENDED);

    //Set pulse position to late.
    ADC_setInterruptPulseMode(REAR_SUSP_SENSOR_ADC_BASE, ADC_PULSE_END_OF_CONV);

    //Power up ADC and delay for one ms.
    ADC_enableConverter(REAR_SUSP_SENSOR_ADC_BASE);
    DEVICE_DELAY_US(1000);
}

//Initialize EPWM
void initRearSuspensionSensorEPWM(void)
{
    //Disable SOCA
    EPWM_disableADCTrigger(REAR_SUSP_SENSOR_EPWM_BASE, REAR_SUSP_SENSOR_EPWM_SOC);

    //Configure SOC to happen on up-count event.
    EPWM_setADCTriggerSource(REAR_SUSP_SENSOR_EPWM_BASE, REAR_SUSP_SENSOR_EPWM_SOC, EPWM_SOC_TBCTR_U_CMPA);
    EPWM_setADCTriggerEventPrescale(REAR_SUSP_SENSOR_EPWM_BASE, REAR_SUSP_SENSOR_EPWM_SOC, 1);

    //Set compare A value to 2048 and period to 4096.
    EPWM_setCounterCompareValue(REAR_SUSP_SENSOR_EPWM_BASE, EPWM_COUNTER_COMPARE_A, 0x0800);
    EPWM_setTimeBasePeriod(REAR_SUSP_SENSOR_EPWM_BASE, 0x1000);

    //Freeze the counter
    EPWM_setTimeBaseCounterMode(REAR_SUSP_SENSOR_EPWM_BASE, EPWM_COUNTER_MODE_STOP_FREEZE);
}

//Initialize ADC SOC and configure the ADCA's SOC0 to be triggered by the ePWM1
void initRearSuspensionSensorADCSOC(void)
{
    //Configure the SOC. The position sensor is connected to B4
    ADC_setupSOC(REAR_SUSP_SENSOR_ADC_BASE, REAR_SUSP_SENSOR_ADC_SOC, ADC_TRIGGER_SW_ONLY,
                 REAR_SUSP_SENSOR_ADC_CHANNEL, 270e-9 * DEVICE_SYSCLK_FREQ);

    //Set the SOC0 to set interrupt 1 flag. Enable interrupt and clear flag.
    ADC_setInterruptSource(REAR_SUSP_SENSOR_ADC_BASE, REAR_SUSP_SENSOR_ADC_INT_NUM, REAR_SUSP_SENSOR_ADC_SOC);
    ADC_enableInterrupt(REAR_SUSP_SENSOR_ADC_BASE, REAR_SUSP_SENSOR_ADC_INT_NUM);
    ADC_clearInterruptStatus(REAR_SUSP_SENSOR_ADC_BASE, REAR_SUSP_SENSOR_ADC_INT_NUM);
}

//Create the interrupt for ADC B Interrupt 1
__interrupt void rearSuspensionISR(void)
{
    //Read the raw result
    rearSensorSample = ADC_readResult(ADCBRESULT_BASE, REAR_SUSP_SENSOR_ADC_SOC);

    //27.3 is the number of adc units in 1 mm of suspension travel.
    rearSensorPosition = (150 - rearSensorSample) / 27.3;

    //Check if the sensor is completely extended.
    rearWheelie = (rearSensorPosition == 150);

    //Clear interrupt flag
    ADC_clearInterruptStatus(REAR_SUSP_SENSOR_ADC_BASE, REAR_SUSP_SENSOR_ADC_INT_NUM);

    //Check for overflow.
    if (true == ADC_getInterruptOverflowStatus(REAR_SUSP_SENSOR_ADC_BASE, REAR_SUSP_SENSOR_ADC_INT_NUM))
    {
        ADC_clearInterruptOverflowStatus(REAR_SUSP_SENSOR_ADC_BASE, REAR_SUSP_SENSOR_ADC_INT_NUM);
        ADC_clearInterruptStatus(REAR_SUSP_SENSOR_ADC_BASE, REAR_SUSP_SENSOR_ADC_INT_NUM);
    }

    //Acknowledge the interrupt
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}

uint16_t getRearSensorPosition(void)
{
    return rearSensorPosition;
}

bool getRearWheelieStatus(void)
{
    return rearWheelie;
}

void initRearSuspensionSensor(void)
{
    //Re-map interrupts in this file to ISR functions in this file.
    Interrupt_register(REAR_SUSP_SENSOR_ADC_INT, &rearSuspensionISR);

    //Set up the ADC and ePWM and initialize the SOC
    initRearSuspensionSensorADC();
    initRearSuspensionSensorEPWM();
    initRearSuspensionSensorADCSOC();

    //Enable ADC interrupt
    Interrupt_enable(REAR_SUSP_SENSOR_ADC_INT);

    //Start ePWM1, enabling SCB and put counter in up-count
    EPWM_enableADCTrigger(REAR_SUSP_SENSOR_EPWM_BASE, REAR_SUSP_SENSOR_EPWM_SOC);
    EPWM_setTimeBaseCounterMode(REAR_SUSP_SENSOR_EPWM_BASE, EPWM_COUNTER_MODE_UP);

}
