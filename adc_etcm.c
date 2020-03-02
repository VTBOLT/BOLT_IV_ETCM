/*
 * adc_etcm.c
 *
 *  Created on: Nov 1, 2019
 *      Author: willc
 */

#include "adc_etcm.h"

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

//Initialize ADC SOC
void initADCSOC(void)
{

}
