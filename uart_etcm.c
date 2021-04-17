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
    GPIO_setPinConfig (UART_GPIO_CFG_SCITX);
    GPIO_setPadConfig(UART_GPIO_SCITX, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(UART_GPIO_SCITX, GPIO_DIR_MODE_OUT);
    //GPIO_setQualificationMode(GPIO_SCITX, GPIO_QUAL_ASYNC);

    //GPIO RX
    //GPIO_setMasterCore(GPIO_SCIRX, GPIO_CORE_CPU1);
    GPIO_setPinConfig (UART_GPIO_CFG_SCIRX);
    GPIO_setPadConfig(UART_GPIO_SCIRX, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(UART_GPIO_SCIRX, GPIO_DIR_MODE_IN);
    //GPIO_setQualificationMode(GPIO_SCIRX, GPIO_QUAL_ASYNC);

    // configure module
    // 8N1
    SCI_setConfig(UART_SCI_BASE, DEVICE_LSPCLK_FREQ, UART_SCI_BAUD, (SCI_CONFIG_WLEN_8 |SCI_CONFIG_STOP_ONE |SCI_CONFIG_PAR_NONE));
    SCI_disableLoopback(UART_SCI_BASE);
    SCI_performSoftwareReset(UART_SCI_BASE);

    // FIFO
    SCI_disableFIFO(UART_SCI_BASE);

    //SCI_disableInterrupt(SCI_BASE, SCI_INT_RXERR);

    //SCI_resetChannels(SCI_BASE);

    SCI_enableModule(UART_SCI_BASE);
}

void initSCIFIFO(void){

    // FIFO
    //SCIinitFIFO();

    // GPIO TX
    GPIO_setMasterCore(UART_GPIO_SCITX, GPIO_CORE_CPU1);
    GPIO_setPinConfig (UART_GPIO_CFG_SCITX);
    GPIO_setPadConfig(UART_GPIO_SCITX, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(UART_GPIO_SCITX, GPIO_DIR_MODE_OUT);
    GPIO_setQualificationMode(UART_GPIO_SCITX, GPIO_QUAL_ASYNC);

    //GPIO RX
    GPIO_setMasterCore(UART_GPIO_SCIRX, GPIO_CORE_CPU1);
    GPIO_setPinConfig (UART_GPIO_CFG_SCIRX);
    GPIO_setPadConfig(UART_GPIO_SCIRX, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(UART_GPIO_SCIRX, GPIO_DIR_MODE_IN);
    GPIO_setQualificationMode(UART_GPIO_SCIRX, GPIO_QUAL_ASYNC);

//    Interrupt_initModule();
//    Interrupt_initVectorTable();

    SCI_performSoftwareReset(UART_SCI_BASE);


    // configure module
    // 8N1
    SCI_setConfig(UART_SCI_BASE, DEVICE_LSPCLK_FREQ, UART_SCI_BAUD, (SCI_CONFIG_WLEN_8 |SCI_CONFIG_STOP_ONE |SCI_CONFIG_PAR_NONE));

    SCI_resetChannels(UART_SCI_BASE);
    SCI_resetRxFIFO(UART_SCI_BASE);
    SCI_resetTxFIFO(UART_SCI_BASE);
    SCI_clearInterruptStatus(UART_SCI_BASE, SCI_INT_TXFF | SCI_INT_RXFF);
    //SCI_disableLoopback(SCI_BASE);
    SCI_enableFIFO(UART_SCI_BASE);

    SCI_enableModule(UART_SCI_BASE);
    SCI_performSoftwareReset(UART_SCI_BASE);
}

void SCItest(void){
    SCI_writeCharBlockingNonFIFO(UART_SCI_BASE, (uint16_t)'F');
    uint32_t index = 0;
    for (index = 0; index <= 1000000; index++);
    SCI_writeCharBlockingNonFIFO(UART_SCI_BASE, (uint16_t)'\n');
    for (index = 0; index <= 1000000; index++);

    //uint16_t rcvd = SCI_readCharBlockingNonFIFO(SCI_BASE);
}

uint8_t SCIgetFifoLength(void){
    return SCI_getRxFIFOStatus(UART_SCI_BASE);
}

void SCIreadFifo(uint16_t *dataBuf, uint8_t FIFOlength)
{
    //dataBuf[1] = 0;
    // get data
    uint8_t index = 0;
    for (index = 0; index < FIFOlength; index++)
    {
        dataBuf[index] = SCI_readCharNonBlocking(UART_SCI_BASE);
    }



    //SCI_writeCharBlockingFIFO(SCI_BASE, )
}


void SCIinitFIFO(void)
{
    SCI_clearInterruptStatus(UART_SCI_BASE, SCI_INT_TXFF);

    SCI_enableFIFO(UART_SCI_BASE);

    SCI_setFIFOInterruptLevel(UART_SCI_BASE, SCI_FIFO_TX0, SCI_FIFO_RX4);

    SCI_clearInterruptStatus(SCIA_BASE, SCI_INT_RXFF);
}
