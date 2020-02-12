/*
 * ecap_etcm.h
 *
 *  Created on: Feb 11, 2020
 *      Author: Tyler
 */

#ifndef ECAP_ETCM_H_
#define ECAP_ETCM_H_

#include "device.h"


/*
 * Globals
 */
extern volatile float signalPeriod;
extern volatile float signalFreq;

void initECAP();
__interrupt void ecap1ISR(void);




#endif /* ECAP_ETCM_H_ */
