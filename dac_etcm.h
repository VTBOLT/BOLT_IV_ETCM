/*
 * dacetcm.h
 *
 *  Created on: Nov 4, 2019
 *      Author: Stephen Welch
 */

#ifndef DAC_ETCM_H_
#define DAC_ETCM_H_

#include "driverlib/dac.h"

#define DAC_BASE_ADDRESS DACA_BASE

void initDAC();
void setDACOutput(uint16_t value);

#endif /* DAC_ETCM_H_ */
