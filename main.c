/**
 * main.c
 */

#include "driverlib.h"
#include "device.h"
#include "IMU.h"


//Main, calls init and run
int main(void)
{

    //Initialize device clock and peripherals.
    Device_init();

    //Disable pin locks and internal pullups
    Device_initGPIO();

    //Initalize the pins used for SCI TX and RX
    initSCIPins();

    //Initialize the GPIO pin for IMU SyncIn.
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
        pulseSyncIn();


        SCI_readCharArray(SCIB_BASE, IMUDataFrame, 18);
    }
}
