/*
 * CAN.h
 *
 *  Created on: Jan 7, 2020
 *      Author: Tyler
 */

#ifndef CAN_ETCM_H_
#define CAN_ETCM_H_

#include "driverlib.h"
#include "device.h"

#define CANA_BAUD 500000        // CANA baud rate in bps
#define TX_MSG_OBJ_1 1          // First TX message object. Can have up to 32 objects in total.
#define TX_MSG_OBJ_1_ID 0x401   // First TX message object ID.

/*
 * Prototypes
 */
void initCAN(void);
/**
 * CANA_transmitMsg1 transmits a CAN V2.0A message onto the bus using module "A", ID 0x401
 *
 * This is a blocking function that returns upon completion of transmission.
 * TODO: Return with status.
 *
 * @param msgData A pointer to a uint16 array containing the message data to be transmitted. Each index can not have over 8 bits of data.
 * @param msgLEN  Integer in the range of 0-8. Indicates message length in bytes.
 */
void CANA_transmitMsg1(uint16_t *msgData, uint16_t msgLEN);

#endif /* CAN_ETCM_H_ */
