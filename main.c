

/**
 * main.c
 */

#include "driverlib.h"
#include "device.h"
#include "string.h"


//Function Prototypes.
__interrupt void scibRXFIFOISR(void);
__interrupt void scibTXFIFOISR(void);
void initSCIBFIFO(void);
void error(void);

//send data
uint16_t sDataB[2];

//receive data
uint16_t rDataB[2];

//check the received data;
uint16_t rDataPointB;



//Main, calls init and run
int main(void)
{

    packetCount = 0;


    //Initialize device clock and peripherals.
    Device_init();

    //Disable pin locks and internal pullups
    Device_initGPIO();


    // GPIO18 is the SCI Rx pin.
    //
    GPIO_setMasterCore(18, GPIO_CORE_CPU1);
    GPIO_setPinConfig(GPIO_18_SCIRXDB);
    GPIO_setDirectionMode(18, GPIO_DIR_MODE_IN);
    GPIO_setPadConfig(18, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(18, GPIO_QUAL_ASYNC);


    // GPIO19 is the SCI Tx pin.
    //
    GPIO_setMasterCore(19, GPIO_CORE_CPU1);
    GPIO_setPinConfig(GPIO_19_SCITXDB);
    GPIO_setDirectionMode(19, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(19, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(19, GPIO_QUAL_ASYNC);

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

    Interrupt_register(INT_SCIB_RX, scibRXFIFOISR);
    Interrupt_register(INT_SCIB_TX, scibTXFIFOISR);



    //Initialize device peripherals.
    initSCIBFIFO();

    for(i = 0; i < 2; i++)
    {
        sDataB[i] = i;
    }

    rDataPointB = sDataB[0];


    Interrupt_enable(INT_SCIB_RX);
    Interrupt_enable(INT_SCIB_TX);

    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9)

    //Enable global interrupt and realtime interrupt
    EINT;
    ERTM;

    for(;;);

}
void error(void)
{
    Example_Fail = 1;
    asm("     ESTOP0"); // Test failed!! Stop!
    for (;;);
}

//Transmit ISR
__interrupt void scibTXFIFOISR(void);
{
    uint16_t i;

    SCI_writeCharArray(SCIB_BASE, sDataB, 2);

    //
    // Increment send data for next cycle
    //
    for(i = 0; i < 2; i++)
    {
        sDataB[i] = (sDataB[i] + 1) & 0x00FF;
    }

    SCI_clearInterruptStatus(SCIB_BASE, SCI_INT_TXFF);

    //
    // Issue PIE ACK
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);
}

//Receive ISR
__interrupt void sciaRXFIFOISR(void)
{
    uint16_t i;

    SCI_readCharArray(SCIB_BASE, rDataB, 2);

    //
    // Check received data
    //
    for(i = 0; i < 2; i++)
    {
        if(rDataB[i] != ((rDataPointB + i) & 0x00FF))
        {
            error();
        }
    }

    rDataPointB = (rDataPointB + 1) & 0x00FF;

    SCI_clearOverflowStatus(SCIB_BASE);

    SCI_clearInterruptStatus(SCIB_BASE, SCI_INT_RXFF);

    //
    // Issue PIE ack
    //
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);

}

void initSCIBFIFO(){

    //
    // 8 char bits, 1 stop bit, no parity. Baud rate is 9600.
    //
    SCI_setConfig(SCIB_BASE, DEVICE_LSPCLK_FREQ, 115200, (SCI_CONFIG_WLEN_8 |
                                                        SCI_CONFIG_STOP_ONE |
                                                        SCI_CONFIG_PAR_NONE));
    SCI_enableModule(SCIB_BASE);
    SCI_enableLoopback(SCIB_BASE);
    SCI_resetChannels(SCIB_BASE);
    SCI_enableFIFO(SCIB_BASE);

    //
    // RX and TX FIFO Interrupts Enabled
    //
    SCI_enableInterrupt(SCIB_BASE, (SCI_INT_RXFF | SCI_INT_TXFF));
    SCI_disableInterrupt(SCIB_BASE, SCI_INT_RXERR);

    SCI_setFIFOInterruptLevel(SCBA_BASE, SCI_FIFO_TX2, SCI_FIFO_RX2);
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
