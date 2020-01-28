/*
 * CAN.h
 *
 *  Created on: Jan 7, 2020
 *      Author: Tyler Shaffer
 */

#ifndef CAN_ETCM_H_
#define CAN_ETCM_H_

#include "driverlib.h"
#include "device.h"

#define CANA_BAUD 500000        // CANA baud rate in bps

#define TX_MSG_OBJ_BASE_ID 0x400   // TX message object ID.

#define TX_MSG_OBJ_1 1          // First TX message object. Can have up to 32 objects in total.
#define TX_MSG_OBJ_1_ID 0x401   // First TX message object ID.

#define MAILBOX1_MASK 0x0001    // Mailbox = msg object

#define CAN_MODULE_BASE CANB_BASE
#define DEFAULT_CAN_MSG_LEN 8

//
// CANA
//
#define DEVICE_GPIO_PIN_CANTXA      4U  // GPIO number for CANTXA
#define DEVICE_GPIO_PIN_CANRXA      5U  // GPIO number for CANRXA
#define DEVICE_GPIO_CFG_CANSILENTA  GPIO_24_GPIO24  // GPIO number for CANA SILENT
#define CANA_SILENTPIN                24

//
// CANB
//
#define DEVICE_GPIO_PIN_CANTXB      12U  // GPIO number for CANTXA
#define DEVICE_GPIO_PIN_CANRXB      17U  // GPIO number for CANRXA


//
// CAN External Loopback
//
#define DEVICE_GPIO_CFG_CANRXA      GPIO_5_CANRXA  // "pinConfig" for CANA RX
#define DEVICE_GPIO_CFG_CANTXA      GPIO_4_CANTXA  // "pinConfig" for CANA TX
#define DEVICE_GPIO_CFG_CANRXB      GPIO_17_CANRXB  // "pinConfig" for CANB RX
#define DEVICE_GPIO_CFG_CANTXB      GPIO_12_CANTXB   // "pinConfig" for CANB TX

/*
 * Prototypes
 */
extern void initCAN(void);

extern bool CANA_transmitMsg(uint16_t *msgData, uint16_t msgLEN, uint16_t mailbox);


#endif /* CAN_ETCM_H_ */
