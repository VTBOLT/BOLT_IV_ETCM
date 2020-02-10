#include "front_suspension_sensor.h"

uint16_t sensorSample;
volatile int16_t sensorPosition;
volatile bool wheelie;

//Initialize and power up the ADC
void initADC(void)
{
    //Set ADC divider to /4.
    ADC_setPrescaler(ADCB_BASE, ADC_CLK_DIV_4_0);

    //Set resolution and signal mode and load the right trims.
    ADC_setMode(ADCB_BASE, ADC_RESOLUTION_12BIT, ADC_MODE_SINGLE_ENDED);

    //Set pulse position to late.
    ADC_setInterruptPulseMode(ADCB_BASE, ADC_PULSE_END_OF_CONV);

    //Power up ADC and delay for one ms.
    ADC_enableConverter(ADCB_BASE);
    DEVICE_DELAY_US(1000);
}

//Initialize EPWM
void initEPWM(void)
{
    //Disable SOCA
    EPWM_disableADCTrigger(EPWM1_BASE, EPWM_SOC_B);

    //Configure SOC to happen on up-count event.
    EPWM_setADCTriggerSource(EPWM1_BASE, EPWM_SOC_B, EPWM_SOC_TBCTR_U_CMPA);
    EPWM_setADCTriggerEventPrescale(EPWM1_BASE, EPWM_SOC_B, 1);

    //Set compare A value to 2048 and period to 4096.
    EPWM_setCounterCompareValue(EPWM1_BASE, EPWM_COUNTER_COMPARE_B, 0x0800);
    EPWM_setTimeBasePeriod(EPWM1_BASE,0x1000);

    //Freeze the counter
    EPWM_setTimeBaseCounterMode(EPWM1_BASE, EPWM_COUNTER_MODE_STOP_FREEZE);
}

//Initialize ADC SOC and configure the ADCA's SOC0 to be triggered by the ePWM1
void initADCSOC(void)
{
    //Configure the SOC. The position sensor is connected to B4
    ADC_setupSOC(ADCB_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_EPWM1_SOCB,
                 ADC_CH_ADCIN6, 15);

    //Set the SOC0 to set interrupt 1 flag. Enable interrupt and clear flag.
    ADC_setInterruptSource(ADCB_BASE, ADC_INT_NUMBER1, ADC_SOC_NUMBER0);
    ADC_enableInterrupt(ADCB_BASE, ADC_INT_NUMBER1);
    ADC_clearInterruptStatus(ADCB_BASE, ADC_INT_NUMBER1);
}

//Create the interrupt for ADC B Interrupt 1
__interrupt void frontSuspensionISR(void)
{
    //Read the raw result
    sensorSample = ADC_readResult(ADCBRESULT_BASE, ADC_SOC_NUMBER0);

    //27.3 is the number of adc units in 1 mm of suspension travel.
    sensorPosition = 150 - (sensorSample/27.3);

    //Check if the sensor is completely extended.
    wheelie = (sensorPosition == 150);

    //Clear interrupt flag
    ADC_clearInterruptStatus(ADCB_BASE, ADC_INT_NUMBER1);

    //Check for overflow.
    if(true == ADC_getInterruptOverflowStatus(ADCB_BASE, ADC_INT_NUMBER1))
    {
        ADC_clearInterruptOverflowStatus(ADCB_BASE, ADC_INT_NUMBER1);
        ADC_clearInterruptStatus(ADCB_BASE, ADC_INT_NUMBER1);
    }

    //Acknowledge the interrupt
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}

uint16_t getSensorPosition(void)
{
    return sensorPosition;
}

bool getWheelie(void)
{
    return wheelie;
}

void initFrontSuspensionSensor(void)
{
    //Re-map interrupts in this file to ISR functions in this file.
    Interrupt_register(INT_ADCB1, &frontSuspensionISR);

    //Set up the ADC and ePWM and initialize the SOC
    initADC();
    initEPWM();
    initADCSOC();

    //Enable ADC interrupt
    Interrupt_enable(INT_ADCB1);


    //Start ePWM1, enabling SOCB and put counter in up-count
    EPWM_enableADCTrigger(EPWM1_BASE, EPWM_SOC_B);
    EPWM_setTimeBaseCounterMode(EPWM1_BASE, EPWM_COUNTER_MODE_UP);

}
