#include "front_suspension_sensor.h"

uint16_t sensorSample;
volatile int16_t sensorPosition;
volatile bool wheelie;

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
                 ADC_CH_ADCIN4, 270e-9 * DEVICE_SYSCLK_FREQ);

    //Set the SOC0 to set interrupt 1 flag. Enable interrupt and clear flag.
    ADC_setInterruptSource(FRONT_SUSP_SENSOR_ADC_BASE, FRONT_SUSP_SENSOR_ADC_INT_NUM, FRONT_SUSP_SENSOR_ADC_SOC);
    ADC_enableInterrupt(FRONT_SUSP_SENSOR_ADC_BASE, FRONT_SUSP_SENSOR_ADC_INT_NUM);
    ADC_clearInterruptStatus(FRONT_SUSP_SENSOR_ADC_BASE, FRONT_SUSP_SENSOR_ADC_INT_NUM);
}

//Create the interrupt for ADC B Interrupt 1
__interrupt void frontSuspensionISR(void)
{
    //Read the raw result
    sensorSample = ADC_readResult(ADCBRESULT_BASE, FRONT_SUSP_SENSOR_ADC_SOC);

    //27.3 is the number of adc units in 1 mm of suspension travel.
    sensorPosition = 150 - (sensorSample / 27.3);

    //Check if the sensor is completely extended.
    wheelie = (sensorPosition == 150);

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
    return sensorPosition;
}

bool getFrontWheelieStatus(void)
{
    return wheelie;
}

void initFrontSuspensionSensor(void)
{
    //Re-map interrupts in this file to ISR functions in this file.
    Interrupt_register(FRONT_SUSP_SENSOR_ADC_INT, &frontSuspensionISR);

    //Set up the ADC and ePWM and initialize the SOC
    initFrontSuspensionSensorADC();
    //initFrontSuspensionEPWM();
    initFrontSuspensionSensorADCSOC();

    //Enable ADC interrupt
    Interrupt_enable(FRONT_SUSP_SENSOR_ADC_INT);

    //Start ePWM1, enabling SCB and put counter in up-count
    EPWM_enableADCTrigger(FRONT_SUSP_SENSOR_EPWM_BASE, EPWM_SOC_B);
    EPWM_setTimeBaseCounterMode(FRONT_SUSP_SENSOR_EPWM_BASE, EPWM_COUNTER_MODE_UP);

}
