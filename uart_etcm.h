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

// IMU
#define GPIO_CFG_SCITX_IMU  GPIO_18_SCITXDB
#define GPIO_CFG_SCIRX_IMU  GPIO_19_SCIRXDB
#define GPIO_SCITX_IMU      18U
#define GPIO_SCIRX_IMU      19U
#define SCI_IMU_BASE        SCIB_BASE
#define SCI_IMU_BAUD        115200U

// DEBUG
#define GPIO_CFG_SCITX_DEBUG  GPIO_42_SCITXDA
#define GPIO_CFG_SCIRX_DEBUG  GPIO_43_SCIRXDA
#define GPIO_SCITX_DEBUG      42U
#define GPIO_SCIRX_DEBUG      43U
#define SCI_DEBUG_BASE        SCIA_BASE
#define SCI_DEBUG_BAUD        115200U

void initSCI(void);
void initSCInoFIFO(void);
void SCItest(void);
void SCIreadFifo(uint16_t *dataBuf, uint8_t FIFOlength);
uint8_t SCIgetFifoLength(void);
void initSCI_IMUwithFIFO(void);
void initSCI_DEBUG();
void SCIwrite(uint32_t SCIbase, const uint16_t *const dataArray, uint16_t dataLength);
void SCIread(uint32_t SCIbase, uint16_t* const array, uint16_t length);


#endif /* UART_ETCM_H_ */
