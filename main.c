

/**
 * main.c
 */

#include "driverlib.h"
#include "device.h"

//Function Prototypes.
void initADC(void);
void initEPWM(void);
void initADCSOC(void);
__interrupt void adcA1ISR(void);

uint16_t sensorSample;
int16_t sensorPosition;
int16_t wheelie;

//Main, calls init and run
int main(void)
{
    //Initialize device clock and peripherals.
    Device_init();

    //Disable pin locks and internal pullups
    Device_initGPIO();

    //Initialize PIE and disable interrupts.
    Interrupt_initModule();

    //Initialize the PIE vector table to the shell ISR
    Interrupt_initVectorTable();

    //Re-map interrupts in this file to ISR functions in this file.
    Interrupt_register(INT_ADCA1, &adcA1ISR);

    //Set up the ADC and ePWM and initialize the SOC
    initADC();
    initEPWM();
    initADCSOC();

    //Enable ADC interrupt
    Interrupt_enable(INT_ADCA1);

    //Enable Global Interrupt (INTM) and realtime interrupt (DBGM)
    EINT;
    ERTM;

    //Start ePWM1, enabling SOCA and put counter in up-count
    EPWM_enableADCTrigger(EPWM1_BASE, EPWM_SOC_A);
    EPWM_setTimeBaseCounterMode(EPWM1_BASE, EPWM_COUNTER_MODE_UP);

    //Loop forever
    while(1)
    {;}

}

//Initialize and power up the ADC
void initADC(void)
{
    //Set ADC divider to /4.
    ADC_setPrescaler(ADCA_BASE, ADC_CLK_DIV_4_0);

    //Set resolution and signal mode and load the right trims.
    ADC_setMode(ADCA_BASE, ADC_RESOLUTION_12BIT, ADC_MODE_SINGLE_ENDED);

    //Set pulse position to late.
    ADC_setInterruptPulseMode(ADCA_BASE, ADC_PULSE_END_OF_CONV);

    //Power up ADC and delay for one ms.
    ADC_enableConverter(ADCA_BASE);
    DEVICE_DELAY_US(1000);
}

//Initialize EPWM
void initEPWM(void)
{
    //Disable SOCA
    EPWM_disableADCTrigger(EPWM1_BASE, EPWM_SOC_A);

    //Configure SOC to happen on up-count event.
    EPWM_setADCTriggerSource(EPWM1_BASE, EPWM_SOC_A, EPWM_SOC_TBCTR_U_CMPA);
    EPWM_setADCTriggerEventPrescale(EPWM1_BASE, EPWM_SOC_A, 1);

    //Set compare A value to 2048 and period to 4096.
    EPWM_setCounterCompareValue(EPWM1_BASE, EPWM_COUNTER_COMPARE_A, 0x0800);
    EPWM_setTimeBasePeriod(EPWM1_BASE,0x1000);

    //Freeze the counter
    EPWM_setTimeBaseCounterMode(EPWM1_BASE, EPWM_COUNTER_MODE_STOP_FREEZE);
}

//Initialize ADC SOC and configure the ADCA's SOC0 to be triggered by the ePWM1
void initADCSOC(void)
{
    //Configure the SOC. The position sensor is connected to A0
    ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_EPWM1_SOCA,
                 ADC_CH_ADCIN14, 140);

    //Set the SOC0 to set interrupt 1 flag. Enable interrupt and clear flag.
    ADC_setInterruptSource(ADCA_BASE, ADC_INT_NUMBER1, ADC_SOC_NUMBER0);
    ADC_enableInterrupt(ADCA_BASE, ADC_INT_NUMBER1);
    ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1);
}

//Create the interrupt for ADC A Interrupt 1
__interrupt void adcA1ISR(void)
{
    //Read the raw result
    sensorSample = ADC_readResult(ADCARESULT_BASE, ADC_SOC_NUMBER0);

    sensorPosition = 150 - (sensorSample/27.3);

    wheelie = (sensorPosition == 150);
    //Clear interrupt flag
    ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1);

    //Check for overflow.
    if(true == ADC_getInterruptOverflowStatus(ADCA_BASE, ADC_INT_NUMBER1))
    {
        ADC_clearInterruptOverflowStatus(ADCA_BASE, ADC_INT_NUMBER1);
        ADC_clearInterruptStatus(ADCA_BASE, ADC_INT_NUMBER1);
    }

    //Acknowledge the interrupt
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}
