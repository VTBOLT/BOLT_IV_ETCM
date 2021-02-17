/*
 * epwm_etcm.h
 *
 *  Created on: Jan 22, 2020
 *      Author: Brady Siegel
 */

#include "epwm.h"
#include "driverlib.h"


#ifndef EPWM_ETCM_H_
#define EPWM_ETCM_H_

void initEPWM1(unsigned int period);
void setCounterCompareAValue1(int value);

void initEPWM2(unsigned int period);
void setCounterCompareAValue2(int value);



#endif /* EPWM_ETCM_H_ */
