/*
 * watchdog.h
 *
 *  Created on: Mar 4, 2020
 *      Author: Tyler
 */

#ifndef WATCHDOG_ETCM_H_
#define WATCHDOG_ETCM_H_

//#include "driverlib.h"
#include "device.h"

#define GPIO_CFG_WATCHDOG   GPIO_7_GPIO7
#define GPIO_WATCHDOG       7

void initWatchdog();
void resetWatchdog();


#endif /* WATCHDOG_ETCM_H_ */
