#include <IMU.h>

void pulseSyncIn(void){
    GPIO_writePin(67, 1);
    GPIO_writePin(67, 0);
}

void initSCI()
{
    
    // SCI Rx pin.
    //
    GPIO_setMasterCore(GPIO_SCIRX, GPIO_CORE_CPU1);
    GPIO_setPinConfig(GPIO_CFG_SCIRX);
    GPIO_setPadConfig(GPIO_SCIRX, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(GPIO_SCIRX, GPIO_DIR_MODE_IN);
    GPIO_setQualificationMode(GPIO_SCIRX, GPIO_QUAL_ASYNC);

    //SCI Tx pin.
    //
    GPIO_setMasterCore(GPIO_SCITX, GPIO_CORE_CPU1);
    GPIO_setPinConfig(GPIO_CFG_SCITX);
    GPIO_setPadConfig(GPIO_SCITX, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(GPIO_SCITX, GPIO_DIR_MODE_OUT);
    GPIO_setQualificationMode(GPIO_SCITX, GPIO_QUAL_ASYNC);

    // 8 char bits, 1 stop bit, no parity. Baud rate is 115200.
    SCI_setConfig(SCI_BASE, DEVICE_LSPCLK_FREQ, SCI_BAUD, (SCI_CONFIG_WLEN_8 |
                  SCI_CONFIG_STOP_ONE | SCI_CONFIG_PAR_NONE));
    
    
    SCI_enableModule(SCIB_BASE);

    SCI_resetChannels(SCIB_BASE);
    SCI_enableFIFO(SCIB_BASE);

    //Initialize the IMU Interrupts.
    initIMUInterrupt();

}

void initIMUInterrupt(void)
{
     
    //Set the RX interrupt handler.
    Interrupt_register(INT_SCIB_RX, readIMUDataISR);

    //enable FIFO Interrupts
    SCI_enableInterrupt(SCI_BASE, SCI_INT_RXFF);
    SCI_disableInterrupts(SCI_BASE,(SCI_INT_RXERR | SCI_FIFO_RX1));

    //set the point where the FIFO_RX flag is fired.
    SCI_setFIFOInterruptLevel(SCI_BASE, SCI_FIFO_TX2, SCI_FIFO_RX1);
    SCI_performSoftwareReset(SCI_BASE)

    //clear buffer
    SCI_resetRxFIFO(SCI_BASE);

    //enable SCI_RX PIE interrupt.
    Interrupt_enable(INT_SCIB_RX);

    //clear PIEACK
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);
}

//Read the data that was received from the IMU
__interrupt void readIMUDataISR(void)
{

    uint16_t i = 0;

    //Variable to store a data frame.
    uint16_t IMUDataFrame[18];


    for(; i < 18; i++)
    {
        IMUDataFrame[i] = SCI_readCharNonBlocking(SCI_BASE)
    }



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
    //
    

    //SCI_enableLoopback(SCIB_BASE);


    //
    // RX and TX FIFO Interrupts Enabled
    //
    SCI_enableInterrupt(SCIB_BASE, (SCI_INT_RXFF | SCI_INT_TXFF));
    SCI_disableInterrupt(SCIB_BASE, SCI_INT_RXERR);

    SCI_setFIFOInterruptLevel(SCIB_BASE, SCI_FIFO_TX2, SCI_FIFO_RX1);
    SCI_performSoftwareReset(SCIB_BASE);

    SCI_resetTxFIFO(SCIB_BASE);
    SCI_resetRxFIFO(SCIB_BASE);

}
