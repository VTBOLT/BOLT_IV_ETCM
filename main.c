

/**
 * main.c
 */

#include "driverlib.h"
#include "device.h"
#include "string.h"

//Function Prototypes.
void initSCI(void);
void initSCIBFIFO(void);
void xmitSCIB(uint16_t a);

int16_t packetCount;
char dataIN[35];
int16_t roll;
int16_t pitch;
int16_t yaw;



//Main, calls init and run
int main(void)
{

    packetCount = 0;


    //Initialize device clock and peripherals.
    Device_init();

    //Disable pin locks and internal pullups
    Device_initGPIO();


    // GPIO18 is the SCI Tx pin.
    //
    GPIO_setMasterCore(18, GPIO_CORE_CPU1);
    GPIO_setPinConfig(GPIO_18_SCITXDB);
    GPIO_setDirectionMode(18, GPIO_DIR_MODE_OUT);
    GPIO_setPadConfig(18, GPIO_PIN_TYPE_STD);
    GPIO_setQualificationMode(18, GPIO_QUAL_ASYNC);


    // GPIO19 is the SCI Rx pin.
    //
    GPIO_setMasterCore(19, GPIO_CORE_CPU1);
    GPIO_setPinConfig(GPIO_19_SCIRXDB);
    GPIO_setDirectionMode(19, GPIO_DIR_MODE_IN);
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
    Interrupt_enableMaster();

    //
    // Initialize SCIB
    //
    initSCI();

    //Initialize the buffer.
    initSCIBFIFO();

    uint16_t receivedNum;

    for(;;){


        while(SCI_getRxFIFOStatus(SCIB_BASE) == SCI_FIFO_RX0)
        {
            xmitSCIB(0x42);
                    ;
        }
        //
        // Get received character
        //
        receivedNum = SCI_readCharBlockingFIFO(SCIB_BASE);
        char recChar = receivedNum;
        dataIN[packetCount] = recChar;
        packetCount = packetCount + 1;
        if(packetCount == 34)
        {
            packetCount = 0;
        }
    }


}

void initSCI(){

    //8 char bits, 1 stop bit, no parity, baud of 115200.
    SCI_setConfig(SCIB_BASE, DEVICE_LSPCLK_FREQ, 115200, (SCI_CONFIG_WLEN_8 |
                                                          SCI_CONFIG_STOP_ONE |
                                                          SCI_CONFIG_PAR_NONE));

    //Disable the test mode where TX & RX are internally connected.
    SCI_disableLoopback(SCIB_BASE);

    SCI_enableFIFO(SCIB_BASE);
    SCI_performSoftwareReset(SCIB_BASE);
    SCI_disableInterrupt(SCIB_BASE, SCI_INT_RXERR);

    SCI_enableInterrupt(SCIA_BASE, SCI_INT_TXRDY);
    SCI_enableInterrupt(SCIB_BASE, SCI_INT_RXRDY_BRKDT);

    //Enable the module
    SCI_enableModule(SCIB_BASE);

}

void xmitSCIB(uint16_t a)
{
    SCI_writeCharNonBlocking(SCIB_BASE, a);
}

void initSCIBFIFO(){

    SCI_clearInterruptStatus(SCIB_BASE, SCI_INT_TXFF);
        SCI_resetTxFIFO(SCIB_BASE);
        SCI_enableFIFO(SCIB_BASE);

        SCI_setFIFOInterruptLevel(SCIB_BASE, SCI_FIFO_TX0, SCI_FIFO_RX4);
        SCI_resetChannels(SCIB_BASE);

        SCI_clearInterruptStatus(SCIB_BASE, SCI_INT_RXFF);
}
