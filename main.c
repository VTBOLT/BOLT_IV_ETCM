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
#include "ecap_etcm.h"

//Function Prototypes.
void init(void);
void run(void);
void initLookup(void);
void CANtest(void);
void LEDflash(void);
void initGPIO(void);
void getIMUdata();

void main(void)
{
    init();
    testECAP();  //does not work for frequency less than 2kHz
    run();

    return;
}

void run(void)
{
    float torque_request = 0.0; // likely to change type
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

       getIMUdata();

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
    initDAC();
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
    CANA_transmitMsg(msg, 4, 1);
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


#define IMU_FRAME_SIZE 18
void getIMUdata(){
    // data container
    volatile uint8_t dataBuffer[IMU_FRAME_SIZE]; // 18 byte frames
    volatile bool IMUframeRcvd = false;

    // make sure FIFO is enabled
    if (!SCI_isFIFOEnabled(SCI_BASE)){
        // return error
        return;
    }
    // clear RX_FIFO
    SCI_resetRxFIFO(SCI_BASE);

    // configure level at which INT flag is thrown
    // RX1 = 1 byte in buffer
    SCI_setFIFOInterruptLevel(SCI_BASE, SCI_FIFO_TX1, SCI_FIFO_RX1);

    // enable RX_FIFO INT
    SCI_enableInterrupt(SCI_BASE, SCI_INT_RXFF);

    // strobe SYNC_IN GPIO

    // wait for buffer fill (INT flag)
    while((HWREGH(SCI_BASE + SCI_O_FFRX) & SCI_FFRX_RXFFINT) != SCI_FFRX_RXFFINT);

    // get data one byte at a time
    volatile uint8_t dataIndex = 0;
    while(!IMUframeRcvd){
        // check buffer
        while(SCI_getRxFIFOStatus(SCI_BASE) == SCI_FIFO_RX0);    // wait for data (will get stuck here)
        // grab byte
        dataBuffer[dataIndex] = SCI_readCharNonBlocking(SCI_BASE);
        dataIndex++;
        // check amount of data bytes received
        if (dataIndex >= IMU_FRAME_SIZE){
            // send buffer data over CAN
            CANA_transmitMsg(dataBuffer, 8, 1);
            CANA_transmitMsg(dataBuffer+8, 8, 2);   // increment pointer
            CANA_transmitMsg(dataBuffer+16, 2, 3);   // increment pointer
            return;
        }
        // loop again
    }


}



//
// End of File
//
