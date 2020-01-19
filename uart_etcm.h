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

#define GPIO_CFG_SCITX  GPIO_56_SCITXDC
#define GPIO_CFG_SCIRX  GPIO_139_SCIRXDC
#define GPIO_SCITX      56U
#define GPIO_SCIRX      139U
#define SCI_BASE        SCIC_BASE
#define SCI_BAUD        9600U

void initSCI(void);
void SCItest(void);
void SCIreadFifo(uint16_t *dataBuf, uint8_t FIFOlength);
uint8_t SCIgetFifoLength(void);
void initSCIFIFO(void);
void SCIinitFIFO(void);

#endif /* UART_ETCM_H_ */
