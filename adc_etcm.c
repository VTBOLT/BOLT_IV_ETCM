/*
 * adc_etcm.c
 *
 *  Created on: Nov 1, 2019
 *      Author: willc
 */

#include "adc_etcm.h"

//Initialize and power up the ADC
void initADC(uint32_t base)
{
    //Set ADC divider to /4.
    ADC_setPrescaler(base, ADC_CLK_DIV_4_0);

    //Set resolution and signal mode and load the right trims.
    ADC_setMode(base, ADC_RESOLUTION_12BIT, ADC_MODE_SINGLE_ENDED);

    //Set pulse position to late.
    ADC_setInterruptPulseMode(base, ADC_PULSE_END_OF_CONV);

    //Power up ADC and delay for one ms.
    ADC_enableConverter(base);
    DEVICE_DELAY_US(1000);
}

////Initialize EPWM
//void initEPWM(void)
//{
//
//}

//Initialize ADC SOC
void initADCSOC(uint32_t base, ADC_Channel adc_channel, ADC_SOCNumber adcsoc0, ADC_SOCNumber adcsoc1, ADC_IntNumber adc_int)
{
    //TODO fix stuff
//    ADC_setupSOC(ADCA_BASE, ADC_SOC_NUMBER0, ADC_TRIGGER_CPU1_TINT0,
//                 ADC_CH_ADCIN0, 15);
    ADC_setupSOC(base, adcsoc0, ADC_TRIGGER_SW_ONLY,
                 adc_channel, 15);
    ADC_setInterruptSource(base, adc_int, adcsoc1);
    ADC_enableInterrupt(base, adc_int);
    ADC_clearInterruptStatus(base, adc_int);
}

uint16_t getADCVal(uint32_t base, uint32_t resultbase, ADC_SOCNumber adcsoc0, ADC_SOCNumber adcsoc1, ADC_IntNumber adc_int)
{
    ADC_forceSOC(base, adcsoc0);
    ADC_forceSOC(base, adcsoc1);

    while(ADC_getInterruptStatus(base, adc_int) == false)
    {
    }
    ADC_clearInterruptStatus(base, adc_int);

    //set variable then return
    uint16_t temp_ADC_readResult = ADC_readResult(resultbase, adcsoc0);
    return temp_ADC_readResult;
}
