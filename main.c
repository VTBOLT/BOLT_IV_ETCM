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

//Function Prototypes.
void init(void);
void run(void);
void initLookup(void);
void CANtest(void);
void LEDflash(void);
void initGPIO(void);

void main(void)
{
    init();
    run();

    return;
}

void run(void)
{
    int torque_request = 0; // likely to change type
    SCI_RxFIFOLevel FIFOlength = 0;

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

        FIFOlength = SCI_getRxFIFOStatus(SCI_BASE);
        //uint16_t UARTdata[FIFOlength];
        //uint16_t UART = SCI_readCharBlockingFIFO(SCI_BASE);

        if (FIFOlength >= SCI_FIFO_RX7){
            uint16_t UARTdata[8];
            SCIreadFifo(UARTdata, 8);
            requestTorque(torque_request);  // dummy debug
            CANA_transmitMsg1(UARTdata, 8);
        }
        //uint16_t rcvd = SCI_readCharBlockingFIFO(SCI_BASE);
        bool FIFOstat = SCI_isFIFOEnabled(SCI_BASE);
        // echo back
        //SCI_writeCharBlockingNonFIFO(SCI_BASE, rcvd);

    }
}

//Initialize, runs all initialization functions
#define GPIO_CFG_BLUE_LED GPIO_31_GPIO31
#define GPIO_BLUE_LED 31

void init(void)
{
    // Initialize device clock and peripherals
    Device_init();
    initGPIO();     // do not move

    initLookup();
    //initADC();
    //initEPWM();
    //initADCSOC();
    initCAN();
    //initSCI();
    initSCIFIFO();
}

//Initialize lookup tables
void initLookup(void)
{
    // Open file containing tables

    // Load tables into ROM
}

/*
 * Send out a test message over CAN
 * ID: 0x401
 */
void CANtest(void)
{
    uint16_t msg[8];
    msg[0] = 0x01;
    msg[1] = 0x23;
    msg[2] = 0x45;
    msg[3] = 0x67;
    msg[4] = 0x89;
    msg[5] = 0xAB;
    msg[6] = 0xCD;
    msg[7] = 0xEF;
    CANA_transmitMsg1(msg, 4);
}

void LEDflash(void){
    GPIO_writePin(GPIO_BLUE_LED, 1);
    // pause for a bit
    unsigned long index = 0; // why do I have to declare this here?
    for (index = 0; index <= 1000000; index++);
    GPIO_writePin(GPIO_BLUE_LED, 0);
    for (index = 0; index <= 1000000; index++);
}

/**
 * Module GPIO inits are in their respective .c file.
 */
void initGPIO(void){
    // GPIOs
    Device_initGPIO();      // must be called first?
    // BLUE_LED
    GPIO_setPinConfig(GPIO_CFG_BLUE_LED);
    GPIO_setPadConfig(GPIO_BLUE_LED, GPIO_PIN_TYPE_STD);        // Push/pull
    GPIO_setDirectionMode(GPIO_BLUE_LED, GPIO_DIR_MODE_OUT);
}

//
// End of File
//
