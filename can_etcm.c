/***
 * CAN DriverLib abstraction for TI C2000 F2837xD
 * Expects to find CAN_A pin configurations in "device.h"
 *
 * Only supports CAN V2.0A 11-bit identifiers
 *
 * Tyler Shaffer, 1/7/2020: Created R0
 * Tyler Shaffer, 1/8/2020: Added basic error and sanity checking. R0 -> R1
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

void initCAN(void)
{
    //
    // Initialize GPIO and configure GPIO pins for CANTX/CANRX
    // on module A
    //
    Device_initGPIO();                          // this may be happening elsewhere
    GPIO_setPinConfig (DEVICE_GPIO_CFG_CANRXA);
    GPIO_setPinConfig (DEVICE_GPIO_CFG_CANTXA);
    // configure silent pin
    GPIO_setPinConfig (DEVICE_GPIO_CFG_CANSILENTA);
    GPIO_setPadConfig(CANA_SILENTPIN, GPIO_PIN_TYPE_STD);
    GPIO_writePin(CANA_SILENTPIN, 0);       // put into normal mode initially
    GPIO_setDirectionMode(CANA_SILENTPIN, GPIO_DIR_MODE_OUT);

    //
    // Initialize the CAN_A controller
    //
    CAN_initModule (CANA_BASE);
    CAN_setBitRate(CANA_BASE, DEVICE_SYSCLK_FREQ, CANA_BAUD, 16);

    //
    // Initialize the transmit message object used for sending CAN messages on ID 0x401.
    // Object ID number 1 has the highest priority, 32 is the lowest.
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
 * This function will return false if a bus error passive state is detected.
 * No action is taken by the code for this state. The hardware will automatically
 * handle an active error state by disabling the CAN TX/RX (bus off).
 *
 * Automatic retransmission in enabled by default.
 *
 * @param msgData A pointer to a uint16 array containing the message data to be transmitted. Each index can not have over 8 bits of data.
 * @param msgLEN  Integer in the range of 0-8. Indicates message length in bytes.
 *
 * @return True if no bus error (likely successful transmission).
 */
bool CANA_transmitMsg1(uint16_t *msgData, uint16_t msgLEN)
{
    //bool CANTXerror = false;
    uint32_t rxErrorCount = 0;
    uint32_t txErrorCount = 0;

    // Ensure that message object 1 is not pending transmission. Prevents overwrite.
    // See page 2493 of tech ref manual and "driverlib/inc/hw_can.h"
    bool mailbox1TxRqst = HWREGH(CANA_BASE + CAN_O_TXRQ_21) && MAILBOX1_MASK;
    if (!mailbox1TxRqst){
        // message not pending transmission
        CAN_sendMessage(CANA_BASE, TX_MSG_OBJ_1_ID, msgLEN, msgData);
    } else {
        // Signal to calling that current message could not be transmitted
        // due to already pending message.
        // Future code could possibly check all 32 mailboxes find one to use. Bear in mind
        // the descending priority.
    }

    // Check for CAN bus error passive state (true). Error counts are not currently handled.
    bool CANTXerror = CAN_getErrorCount(CANA_BASE, &rxErrorCount, &txErrorCount);

    //
    // Poll TxOk bit in CAN_ES register to check completion of transmission
    //
    /* while (((HWREGH(CANA_BASE + CAN_O_ES) & CAN_ES_TXOK)) != CAN_ES_TXOK)
    {
    }
    */

    return !CANTXerror;
}

