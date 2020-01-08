/***
 * CAN DriverLib abstraction for TI C2000 F2837xD
 * Expects to find CAN_A pin configurations in "device.h"
 *
 * Only supports CAN V2.0A 11-bit identifiers
 *
 * Tyler Shaffer, 1/7/2020: Created R0
 *
 */

//
// Included Files
//
#include <can_etcm.h>

/**
 * CAN Init
 *
 * Call once to initialize CAN A module.
 *
 */

void CAN_init(void)
{
    //
    // Initialize GPIO and configure GPIO pins for CANTX/CANRX
    // on module A
    //
    Device_initGPIO();
    GPIO_setPinConfig (DEVICE_GPIO_CFG_CANRXA);
    GPIO_setPinConfig (DEVICE_GPIO_CFG_CANTXA);

    //
    // Initialize the CAN_A controller
    //
    CAN_initModule (CANA_BASE);
    CAN_setBitRate(CANA_BASE, DEVICE_SYSCLK_FREQ, CANA_BAUD, 16);

    //
    // Initialize the transmit message object used for sending CAN messages on ID 0x401.
    // Message Object Parameters:
    //      CAN Module: A
    //      Message Object ID Number: 1
    //      Message Identifier: 0x401
    //      Message Frame: Standard
    //      Message Type: Transmit
    //      Message ID Mask: 0x0
    //      Message Object Flags: None
    //      Message Data Length: 8
    //
    CAN_setupMessageObject(CANA_BASE, TX_MSG_OBJ_1, TX_MSG_OBJ_1_ID,
                           CAN_MSG_FRAME_STD, CAN_MSG_OBJ_TYPE_TX, 0,
                           CAN_MSG_OBJ_NO_FLAGS, 8);

    //
    // Start CAN module A operations
    //
    CAN_startModule(CANA_BASE);
}

/**
 * CANA_transmitMsg1 transmits a CAN V2.0A message onto the bus using module "A", ID 0x401
 *
 * This is a blocking function that returns upon completion of transmission.
 * TODO: Return with status.
 *
 * @param msgData A pointer to a uint16 array containing the message data to be transmitted. Each index can not have over 8 bits of data.
 * @param msgLEN  Integer in the range of 0-8. Indicates message length in bytes.
 */
void CANA_transmitMsg1(uint16_t *msgData, uint16_t msgLEN)
{

    CAN_sendMessage(CANA_BASE, TX_MSG_OBJ_1_ID, msgLEN, msgData);

    //
    // Poll TxOk bit in CAN_ES register to check completion of transmission
    //
    while (((HWREGH(CANA_BASE + CAN_O_ES) & CAN_ES_TXOK)) != CAN_ES_TXOK)
    {
    }
}

