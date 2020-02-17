

/**
 * main.c
 */

#include "driverlib.h"
#include "device.h"

//Include sensor files.
#include "front_suspension_sensor.h"



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

    initFrontSuspensionSensor();

    //Loop forever
    while(1)
    {
        ADC_forceSOC(ADCB_BASE, ADC_SOC_NUMBER0);
        //DEVICE_DELAY_US(1000);

    }

}
