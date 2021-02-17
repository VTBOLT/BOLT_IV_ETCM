/*
 * gpio_etcm.h
 *
 *  Created on: Feb 4, 2020
 *      Author: Brady Siegel
 */

#include "gpio.h"
#include "driverlib.h"

#ifndef GPIO_ETCM_H_
#define GPIO_ETCM_H_

typedef enum {UP, DOWN, NONE} buttonState;

void initGPIO();
void buttonStateMachine(char status, buttonState* b);
void sampleGPIO(char* status);

#endif /* GPIO_ETCM_H_ */
