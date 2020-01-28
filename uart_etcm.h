/*
 * uart_etcm.h
 *
 *  Created on: Jan 19, 2020
 *      Author: Tyler
 */

#ifndef UART_ETCM_H_
#define UART_ETCM_H_

#include "device.h"
#include "driverlib.h"

#define GPIO_CFG_SCITX  GPIO_18_SCITXDB
#define GPIO_CFG_SCIRX  GPIO_19_SCIRXDB
#define GPIO_SCITX      18U
#define GPIO_SCIRX      19U

#define SCI_BASE        SCIB_BASE
#define SCI_BAUD        115200U

void initSCI(void);
void SCItest(void);
void SCIreadFifo(uint16_t *dataBuf, uint8_t FIFOlength);
uint8_t SCIgetFifoLength(void);
void initSCIwithFIFO(void);


#endif /* UART_ETCM_H_ */
