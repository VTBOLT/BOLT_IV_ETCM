/*
 * gpio_etcm.c
 *
 *  Created on: Feb 4, 2020
 *      Author: Brady Siegel
 */
#include "gpio_etcm.h"

void initGPIO()
{
    GPIO_setDirectionMode(67, GPIO_DIR_MODE_OUT);
    GPIO_setDirectionMode(111, GPIO_DIR_MODE_OUT);
    GPIO_setDirectionMode(60, GPIO_DIR_MODE_OUT);
    GPIO_setDirectionMode(61, GPIO_DIR_MODE_OUT);
    GPIO_setDirectionMode(123, GPIO_DIR_MODE_OUT);
    GPIO_setDirectionMode(122, GPIO_DIR_MODE_OUT);

    GPIO_writePin(67, 0);
    GPIO_writePin(111, 0);
    GPIO_writePin(60, 0);
    GPIO_writePin(61, 0);
    GPIO_writePin(123, 0);
    GPIO_writePin(122, 0);


    GPIO_setPinConfig(GPIO_0_EPWM1A);
    GPIO_setPinConfig(GPIO_2_EPWM2A);

    //GPIO_setDirectionMode(, GPIO_DIR_MODE_IN);
}
void sampleGPIO(char* status)
{
    *status <<= 1;
    *status += 1;
}
void buttonStateMachine(char status, buttonState* b)
{
    switch(*b)
    {
    case UP:
        if (status == 0xFFFF)
        {
            *b = DOWN;
        }
        break;
    case DOWN:
        if (status == 0x0000)
        {
            *b = UP;
        }
        break;
    }
}

