/*
 * sci_etcm.c
 *
 *  Created on: Nov 4, 2019
 *      Author: Brady Siegel
 */

#include "sci_etcm.h"

//void initSCI(uint32_t base, uint32_t lspclkHz, uint32_t baud)
//{
//    //
//    // GPIO28 is the SCI Rx pin.
//    //
//    GPIO_setMasterCore(28, GPIO_CORE_CPU1);
//    GPIO_setPinConfig(GPIO_28_SCIRXDA);
//    GPIO_setDirectionMode(28, GPIO_DIR_MODE_IN);
//    GPIO_setPadConfig(28, GPIO_PIN_TYPE_STD);
//    GPIO_setQualificationMode(28, GPIO_QUAL_ASYNC);
//
//    //
//    // GPIO29 is the SCI Tx pin.
//    //
//    GPIO_setMasterCore(29, GPIO_CORE_CPU1);
//    GPIO_setPinConfig(GPIO_29_SCITXDA);
//    GPIO_setDirectionMode(29, GPIO_DIR_MODE_OUT);
//    GPIO_setPadConfig(29, GPIO_PIN_TYPE_STD);
//    GPIO_setQualificationMode(29, GPIO_QUAL_ASYNC);
//
//    //SCI_setConfig(base, lspclkHz, baud, SCI_CONFIG_WLEN_8 | SCI_CONFIG_STOP_ONE | SCI_CONFIG_PARITY_NONE);
//    SCI_disableLoopback(SCIA_BASE);
//
//    SCI_enableFIFO(SCIA_BASE);
//    SCI_performSoftwareReset(SCIA_BASE);
//
//    SCI_disableInterrupt(SCIA_BASE, SCI_INT_RXERR);
//
//    SCI_enableInterrupt(SCIA_BASE, SCI_INT_RXRDY_BRKDT);
//
//    SCI_enableModule(SCIA_BASE);
//}
void initSCIFIFO(uint32_t base)
{
    SCI_clearInterruptStatus(SCIA_BASE, SCI_INT_TXFF);
    SCI_resetTxFIFO(SCIA_BASE);
    SCI_enableFIFO(SCIA_BASE);

    SCI_setFIFOInterruptLevel(SCIA_BASE, SCI_FIFO_TX0, SCI_FIFO_RX4);
    SCI_resetChannels(SCIA_BASE);

    SCI_clearInterruptStatus(SCIA_BASE, SCI_INT_RXFF);
}
void sendData(uint32_t base, uint16_t ch)
{
    SCI_writeCharNonBlocking(base, ch);
}
char getData(uint32_t base)
{
    while(SCI_getRxFIFOStatus(base) == SCI_FIFO_RX0);
    return SCI_readCharBlockingFIFO(base);
}
