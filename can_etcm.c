/***
 * CAN DriverLib abstraction for TI C2000 F2837xD
 * Expects to find CAN_A pin configurations in "device.h"
 *
 * Only supports CAN V2.0A 11-bit identifiers
 *
 * Tyler Shaffer, 1/7/2020: Created R0
 * Tyler Shaffer, 1/8/2020: Added basic error and sanity checking. R0 -> R1
 * Tyler Shaffer, 1/19/2020: Updated to use CANB for just launchpad, no PCB. Tested and working.
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
    // Initialize GPIO and configure GPIO pins for CANTX/CANRX
    // on module A
    GPIO_setPinConfig (DEVICE_GPIO_CFG_CANRXB);
    GPIO_setPinConfig (DEVICE_GPIO_CFG_CANTXB);
    // configure silent pin
    GPIO_setPinConfig (DEVICE_GPIO_CFG_CANSILENTA);
    GPIO_setPadConfig(CANA_SILENTPIN, GPIO_PIN_TYPE_STD);
    GPIO_writePin(CANA_SILENTPIN, 0);       // put into normal mode initially
    GPIO_setDirectionMode(CANA_SILENTPIN, GPIO_DIR_MODE_OUT);

    // Initialize the CAN_A controller
    CAN_initModule (CAN_MODULE_BASE);
    CAN_setBitRate(CAN_MODULE_BASE, DEVICE_SYSCLK_FREQ, CANA_BAUD, 16);

    //
    // Initialize the transmit message object used for sending CAN messages on ID 0x401.
    // Object ID number 1 has the highest priority, 32 is the lowest.
    // Message Object Parameters:
    //      CAN Module: A
    //      Message Object ID Number: User
    //      Message Identifier: 0x400 + mailbox
    //      Message Frame: Standard
    //      Message Type: Transmit
    //      Message ID Mask: 0x0
    //      Message Object Flags: None
    //      Message Data Length: User
    //
    CAN_setupMessageObject(CAN_MODULE_BASE, TX_MSG_OBJ_1, TX_MSG_OBJ_1_ID,
                           CAN_MSG_FRAME_STD, CAN_MSG_OBJ_TYPE_TX, 0,
                           CAN_MSG_OBJ_NO_FLAGS, DEFAULT_CAN_MSG_LEN);

    // Start CAN module A operations
    CAN_startModule(CAN_MODULE_BASE);
}

/**
 * CANA_transmitMsg(uint16_t *msgData, uint16_t msgLEN, uint16_t mailbox) transmits a CAN V2.0A message
 * onto the bus using module "A", ID 0x400 + mailbox number
 *
 * This function will return false if a bus error passive state is detected.
 * No action is taken by the code for this state. The hardware will automatically
 * handle an active error state by disabling the CAN TX/RX (bus off).
 *
 * Automatic retransmission is enabled by default.
 *
 * @param msgData A pointer to a uint16 array containing the message data to be transmitted. Each index can not have over 8 bits of data.
 * @param msgLEN  Integer in the range of 0-8. Indicates message length in bytes.
 * @param mailbox The mailbox number. 1-32
 *
 * @return True if no bus error (likely successful transmission).
 */
bool CANA_transmitMsg(uint16_t *msgData, uint16_t msgLEN, uint16_t mailbox)
{
    //bool CANTXerror = false;
    uint32_t rxErrorCount = 0;
    uint32_t txErrorCount = 0;

    // check for mailbox number or size change and update if needed.
    static uint16_t msgLENold = DEFAULT_CAN_MSG_LEN;
    static uint16_t mailboxOld = MAILBOX1_MASK;
    if ((msgLEN != msgLENold) | (mailbox != mailboxOld))
    {
        CAN_setupMessageObject(CAN_MODULE_BASE, mailbox, (TX_MSG_OBJ_BASE_ID + mailbox),
                               CAN_MSG_FRAME_STD, CAN_MSG_OBJ_TYPE_TX, 0,
                               CAN_MSG_OBJ_NO_FLAGS,
                               msgLEN);
    }
    msgLENold = msgLEN;
    mailboxOld = mailbox;

    // Ensure that message object 1 is not pending transmission. Prevents overwrite.
    // See page 2493 of tech ref manual and "driverlib/inc/hw_can.h"
    /*uint32_t mailboxBit = 1;
    if (mailbox > 1){
        mailboxBit = mailboxBit << mailbox-1;
    }
    bool mailboxTxRqst = HWREGH(CAN_MODULE_BASE + CAN_O_TXRQ_21) && mailboxBit;
    if (!mailboxTxRqst)
    {
        // message not pending transmission
        CAN_sendMessage(CAN_MODULE_BASE, TX_MSG_OBJ_BASE_ID + mailbox, msgLEN, msgData);
    }
    else
    {
        // Signal to calling that current message could not be transmitted
        // due to already pending message.
        // Future code could possibly check all 32 mailboxes find one to use. Bear in mind
        // the descending priority.
    }
    */

    // Wait for previous mailboxes to be transmitted
    // Can we queue??
    while (HWREGH(CAN_MODULE_BASE + CAN_O_TXRQ_21));
    CAN_sendMessage(CAN_MODULE_BASE, TX_MSG_OBJ_BASE_ID + mailbox, msgLEN, msgData);


    // Check for CAN bus error passive state (true). Error counts are not currently handled.
    bool CANTXerror = CAN_getErrorCount(CAN_MODULE_BASE, &rxErrorCount, &txErrorCount);

    //
    // Poll TxOk bit in CAN_ES register to check completion of transmission
    //
    /* while (((HWREGH(CANA_BASE + CAN_O_ES) & CAN_ES_TXOK)) != CAN_ES_TXOK)
     {
     }
     */

    return !CANTXerror;
}
