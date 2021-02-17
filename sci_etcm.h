/*
 * sci_etcm.h
 *
 *  Created on: Nov 4, 2019
 *      Author: Brady Siegel
 */

#ifndef SCI_ETCM_H_
#define SCI_ETCM_H_

#include "driverlib.h"

//void initSCI(uint32_t base, uint32_t lspclkHz, uint32_t baud);
void initSCIFIFO(uint32_t base);
void sendData(uint32_t base, uint16_t ch);
char getData(uint32_t base);

#endif /* SCI_ETCM_H_ */
