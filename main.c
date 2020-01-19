/**
 * main.c
 */

#include "driverlib.h"
#include "device.h"
#include "string.h"

//Function Prototypes.

__interrupt void scibTXFIFOISR(void);
__interrupt void readIMUData(void);
void initSCIBFIFO(void);
void toggleSyncIn(void);
void enableGPIO(void);

float yaw;
float pitch;



//Main, calls init and run
int main(void)
{

    //Initialize device clock and peripherals.
    Device_init();

    //Disable pin locks and internal pullups
    Device_initGPIO();

    // GPIO18 is the SCI Rx pin.
    //
    GPIO_setMasterCore(18, GPIO_CORE_CPU1);
    GPIO_setPinConfig(GPIO_18_SCITXDB);
    GPIO_setDirectionMode(18, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(18, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(18, GPIO_QUAL_ASYNC);

    // GPIO19 is the SCI Tx pin.
    //
    GPIO_setMasterCore(19, GPIO_CORE_CPU1);
    GPIO_setPinConfig(GPIO_19_SCIRXDB);
    GPIO_setDirectionMode(19, GPIO_DIR_MODE_IN);
    GPIO_setPadConfig(19, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(19, GPIO_QUAL_ASYNC);

    enableGPIO();
    //
    // Initialize PIE and clear PIE registers. Disables CPU interrupts.
    //
    Interrupt_initModule();

    // Initialize the PIE vector table with pointers to the shell Interrupt
    // Service Routines (ISR).
    //
    Interrupt_initVectorTable();

    //
    // Enables CPU interrupts
    //

    Interrupt_register(INT_SCIB_RX, readIMUData);

    //Initialize the buffer.
    initSCIBFIFO();


    Interrupt_enable(INT_SCIB_RX);

    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);

    //
    // Enable Global Interrupt (INTM) and realtime interrupt (DBGM)
    //
    EINT;
    ERTM;

    //
    // IDLE loop. Just sit and loop forever (optional):
    //

    uint16_t IMUDataFrame[18];
    for (;;)
    {
        toggleSyncIn();


        SCI_readCharArray(SCIB_BASE, IMUDataFrame, 18);
    }
}

void toggleSyncIn(void){
    GPIO_writePin(67, 1);
    GPIO_writePin(67, 0);
}

//Read the data that was received from the IMU
__interrupt void readIMUData(void)
{

    //Variable to store a data frame.
    uint16_t IMUDataFrame[18];

    SCI_readCharArray(SCIB_BASE, IMUDataFrame, 4);



    SCI_clearOverflowStatus(SCIB_BASE);

    SCI_clearInterruptStatus(SCIB_BASE, SCI_INT_RXFF);

    //
    // Issue PIE ack
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);

}

void enableGPIO()
{
    GPIO_setPadConfig(67, GPIO_PIN_TYPE_PULLUP); //Enable pullup on GPIO67
    GPIO_setPinConfig(GPIO_67_GPIO67);
    GPIO_setDirectionMode(67, GPIO_DIR_MODE_OUT);
}

void initSCIBFIFO()
{
    //
    // 8 char bits, 1 stop bit, no parity. Baud rate is 115200.
    //
    SCI_setConfig(SCIB_BASE, DEVICE_LSPCLK_FREQ, 115200, (SCI_CONFIG_WLEN_8 |
    SCI_CONFIG_STOP_ONE | SCI_CONFIG_PAR_NONE));

    SCI_enableModule(SCIB_BASE);
    //SCI_enableLoopback(SCIB_BASE);
    SCI_resetChannels(SCIB_BASE);
    SCI_enableFIFO(SCIB_BASE);

    //
    // RX and TX FIFO Interrupts Enabled
    //
    SCI_enableInterrupt(SCIB_BASE, (SCI_INT_RXFF | SCI_INT_TXFF));
    SCI_disableInterrupt(SCIB_BASE, SCI_INT_RXERR);

    SCI_setFIFOInterruptLevel(SCIB_BASE, SCI_FIFO_TX2, SCI_FIFO_RX4);
    SCI_performSoftwareReset(SCIB_BASE);

    SCI_resetTxFIFO(SCIB_BASE);
    SCI_resetRxFIFO(SCIB_BASE);

#ifdef AUTOBAUD
//
// Perform an autobaud lock.
// SCI expects an 'a' or 'A' to lock the baud rate.
//
    SCI_lockAutobaud(SCIB_BASE);
#endif
}
