

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
void enableGPIO(void);

int16_t packetCount;
char dataBuffer[18];
float roll;
float pitch;
float yaw;



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

    enableGPIO();


    GPIO_writePin(67,0);
    for(;;){


        GPIO_writePin(67,1);


        int i = 0;
        //
        // Get received data
        //
        uint16_t thing = SCI_getRxStatus(SCIB_BASE);
        uint32_t inter = SCI_getInterruptStatus(SCIB_BASE);
        for( ; i < 16; i++)
        {
            uint16_t temp = SCI_readCharBlockingFIFO(SCIB_BASE);
            dataBuffer[i] = temp;
        }
        yaw = ((dataBuffer[4] << 23) || (dataBuffer[5] << 15) ||(dataBuffer[6] << 7) || dataBuffer[7]);
        pitch = (dataBuffer[4] || dataBuffer[5]);
        roll = (dataBuffer[6] || dataBuffer[7]);
        GPIO_writePin(67,0);
  }

}

void enableGPIO(){
    GPIO_setPadConfig(67, GPIO_PIN_TYPE_PULLUP); //Enable pullup on GPIO22
    GPIO_setPinConfig(GPIO_67_GPIO67);
    GPIO_setDirectionMode(67, GPIO_DIR_MODE_OUT);
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
