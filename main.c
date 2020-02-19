//#############################################################################
//
// FILE:   main.c
//
// TITLE:  BOLT IV ETCM
//
//#############################################################################
// Contributors:
//  Will Campbell
//  Quinton Miller
//  Josh Collins
//  Tyler Shaffer
//#############################################################################
// Tyler Shaffer, 1/8/2020: Imported can_etcm.h, noticed that a new version of
// C2000ware exists also.
// Tyler Shaffer, 1/8/2020: Checked CAN_A pins.
//
//#############################################################################
//Notes:
//  Follow the GNOME style guide
//
//  Anything related to individual peripherals should be in corresponding .c and .h files
//      that are then included below
//
//
//#############################################################################

// Included peripheral files
#include <can_etcm.h>
#include "adc_etcm.h"
#include "dac_etcm.h"
#include <uart_etcm.h>
#include <timer_etcm.h>
#include <leds_etcm.h>
#include <IMU.h>

//***********************
// Function Prototypes
//***********************
void init(void);
void run(void);
void initLookup(void);
void CANtest(void);
void LEDflash(void);
void initGPIO(void);
void getIMUdata();
__interrupt void SCI_ISR(void);
void initSCIinterrupt(void);
void initInterrupts(void);
void initIMUinterrupt(void);
void getIMUdataINT(void);
void updateIMUbuffer(void);
void displayIMU_CAN(void);

void main(void)
{
    init();
    run();

    return;
}

void run(void)
{
    int torque_request = 0; // likely to change type
    // start the timer
    startTimer0();
    while (1)
    {
        // Pull in sensor data to local variables
        // This will use getters inside the peripheral .h/.c files

        // Follow lookup table logic
        // Specific logic TBD

        // Carry out any calculations

        // Send torque request to motor
        requestTorque(torque_request);

        // Send a test CANmsg
        //CANtest();

        // Flash the blue LED
        LEDflash();

        // uart test
        //SCItest();

        //getIMUdata();
        strobeIMUSyncIn();

        float temp;
        temp = getIMUPitch();
        // after 5 seconds, reduce period to 500mS
        if (cpuTimer0IntCount >= 5)
        {
            reloadTimer0(500);
        }

    }
}

//Initialize, runs all initialization functions
void init(void)
{
    // Initialize device clock and peripherals
    Device_init();
    initGPIO();     // do not move
    initLEDS();

    initTimer0();
    initIMUTransfer();
    initInterrupts();
}

void LEDflash(void)
{
    GPIO_writePin(GPIO_BLUE_LED, 1);
    // pause for a bit
    unsigned long index = 0; // why do I have to declare this here?
    for (index = 0; index <= 1000000; index++)
        ;
    GPIO_writePin(GPIO_BLUE_LED, 0);
    for (index = 0; index <= 1000000; index++)
        ;
}

/**
 * SCI_FIFO_RX will throw an interrupt every time there is at least
 * one byte in the buffer. This function is called by the SCI_ISR()
 * to fetch the data and store it in a global container.
 *
 * TODO: Verify frame integrity (start-of-header, checksum)
 */

void initInterrupts(void)
{
// Initialize PIE and clear PIE registers. Disables CPU interrupts.
    Interrupt_initModule();
// Initialize the PIE vector table with pointers to the shell Interrupt
    Interrupt_initVectorTable();

//*******************************
// Interrupt init calls go here
//-------------------------------
    initIMUinterrupt();
    initTimer0Interrupt();

//*******************************

// enable CPU interrupts
// this should always be last
// see Pg. 100 of Tech Ref Manual
    Interrupt_enableMaster();
//EINT;
//ERTM;
}
/**
 * Module GPIO inits are in their respective .c file.
 */
void initGPIO(void)
{
    Device_initGPIO();      // must be called first?

    //********
    // GPIOs
    //--------

    // SYNC_IN
    GPIO_setPinConfig (GPIO_CFG_SYNC_IN);
    GPIO_setPadConfig(GPIO_SYNC_IN, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(GPIO_SYNC_IN, GPIO_DIR_MODE_OUT);
    GPIO_writePin(GPIO_SYNC_IN, 0); // default state

    //********

}
//
// End of File
//
