/*
 * uart.c
 *
 *  Created on: Jan 19, 2020
 *      Author: Tyler
 */

#include <uart_etcm.h>

void initSCI(void){
    // GPIO TX
    //GPIO_setMasterCore(GPIO_SCITX, GPIO_CORE_CPU1);
    GPIO_setPinConfig (GPIO_CFG_SCITX);
    GPIO_setPadConfig(GPIO_SCITX, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(GPIO_SCITX, GPIO_DIR_MODE_OUT);
    //GPIO_setQualificationMode(GPIO_SCITX, GPIO_QUAL_ASYNC);

    //GPIO RX
    //GPIO_setMasterCore(GPIO_SCIRX, GPIO_CORE_CPU1);
    GPIO_setPinConfig (GPIO_CFG_SCIRX);
    GPIO_setPadConfig(GPIO_SCIRX, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(GPIO_SCIRX, GPIO_DIR_MODE_IN);
    //GPIO_setQualificationMode(GPIO_SCIRX, GPIO_QUAL_ASYNC);

    // configure module
    // 8N1
    SCI_setConfig(SCI_BASE, DEVICE_LSPCLK_FREQ, SCI_BAUD, (SCI_CONFIG_WLEN_8 |SCI_CONFIG_STOP_ONE |SCI_CONFIG_PAR_NONE));
    SCI_disableLoopback(SCI_BASE);
    SCI_performSoftwareReset(SCI_BASE);

    // FIFO
    SCI_disableFIFO(SCI_BASE);

    //SCI_disableInterrupt(SCI_BASE, SCI_INT_RXERR);

    //SCI_resetChannels(SCI_BASE);

    SCI_enableModule(SCI_BASE);
}

void initSCIFIFO(void){

    // FIFO
    //SCIinitFIFO();

    // GPIO TX
    GPIO_setMasterCore(GPIO_SCITX, GPIO_CORE_CPU1);
    GPIO_setPinConfig (GPIO_CFG_SCITX);
    GPIO_setPadConfig(GPIO_SCITX, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(GPIO_SCITX, GPIO_DIR_MODE_OUT);
    GPIO_setQualificationMode(GPIO_SCITX, GPIO_QUAL_ASYNC);

    //GPIO RX
    GPIO_setMasterCore(GPIO_SCIRX, GPIO_CORE_CPU1);
    GPIO_setPinConfig (GPIO_CFG_SCIRX);
    GPIO_setPadConfig(GPIO_SCIRX, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(GPIO_SCIRX, GPIO_DIR_MODE_IN);
    GPIO_setQualificationMode(GPIO_SCIRX, GPIO_QUAL_ASYNC);

    Interrupt_initModule();
    Interrupt_initVectorTable();

    SCI_performSoftwareReset(SCI_BASE);


    // configure module
    // 8N1
    SCI_setConfig(SCI_BASE, DEVICE_LSPCLK_FREQ, SCI_BAUD, (SCI_CONFIG_WLEN_8 |SCI_CONFIG_STOP_ONE |SCI_CONFIG_PAR_NONE));

    SCI_resetChannels(SCI_BASE);
    SCI_resetRxFIFO(SCI_BASE);
    SCI_resetTxFIFO(SCI_BASE);
    SCI_clearInterruptStatus(SCI_BASE, SCI_INT_TXFF | SCI_INT_RXFF);
    //SCI_disableLoopback(SCI_BASE);
    SCI_enableFIFO(SCI_BASE);

    SCI_enableModule(SCI_BASE);
    SCI_performSoftwareReset(SCI_BASE);
}

void SCItest(void){
    SCI_writeCharBlockingNonFIFO(SCI_BASE, (uint16_t)'F');
    uint32_t index = 0;
    for (index = 0; index <= 1000000; index++);
    SCI_writeCharBlockingNonFIFO(SCI_BASE, (uint16_t)'\n');
    for (index = 0; index <= 1000000; index++);

    //uint16_t rcvd = SCI_readCharBlockingNonFIFO(SCI_BASE);
}

uint8_t SCIgetFifoLength(void){
    return SCI_getRxFIFOStatus(SCI_BASE);
}

void SCIreadFifo(uint16_t *dataBuf, uint8_t FIFOlength)
{
    //dataBuf[1] = 0;
    // get data
    uint8_t index = 0;
    for (index = 0; index < FIFOlength; index++)
    {
        dataBuf[index] = SCI_readCharNonBlocking(SCI_BASE);
    }



    //SCI_writeCharBlockingFIFO(SCI_BASE, )
}


void SCIinitFIFO(void)
{
    SCI_clearInterruptStatus(SCI_BASE, SCI_INT_TXFF);

    SCI_enableFIFO(SCI_BASE);
    // configure level at which INT flag is thrown
    // RX1 = 1 byte in buffer
    SCI_setFIFOInterruptLevel(SCI_BASE, SCI_FIFO_TX1, SCI_FIFO_RX1);

    // enable RX_FIFO INT
    SCI_enableInterrupt(SCI_BASE, SCI_INT_RXFF);

    SCI_clearInterruptStatus(SCIA_BASE, SCI_INT_RXFF);
}
