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
#define TX_MSG_OBJ_1 1          // First TX message object. Can have up to 32 objects in total.
#define TX_MSG_OBJ_1_ID 0x401   // First TX message object ID.
#define MAILBOX1_MASK 0x0001    // Mailbox = msg object

/*
 * Prototypes
 */
void initCAN(void);

bool CANA_transmitMsg1(uint16_t *msgData, uint16_t msgLEN);

#endif /* CAN_ETCM_H_ */
