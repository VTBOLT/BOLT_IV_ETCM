/*
 * leds_etcm.h
 *
 *  Created on: Jan 21, 2020
 *      Author: Tyler
 */

#ifndef LEDS_ETCM_H_
#define LEDS_ETCM_H_

//
// Included Files
//
#include "driverlib.h"
#include "device.h"

#define GPIO_CFG_RED_LED GPIO_34_GPIO34
#define GPIO_RED_LED 34

#define GPIO_CFG_BLUE_LED GPIO_31_GPIO31
#define GPIO_BLUE_LED 31

extern void toggleRedLed(void);
extern void initLEDS(void);
#endif /* LEDS_ETCM_H_ */
