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

#endif /* CAN_ETCM_H_ */
