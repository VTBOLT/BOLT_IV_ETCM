/******************************************************************************
*
* FILE:   main.c
*
* TITLE:  BOLT IV ETCM
*
*******************************************************************************
* Contributors:
*  Will Campbell
*  Quinton Miller
*  Josh Collins
*  Tyler Shaffer
*******************************************************************************
* Tyler Shaffer, 1/8/2020: Imported can_etcm.h, noticed that a new version of
* C2000ware exists also.
* Tyler Shaffer, 1/8/2020: Checked CAN_A pins.
* Will Campbell, 8/29/20: Commented out extraneous code, added notes. I haven't
* worked on this code in a while so no guarantees, it will still need to be added to
* and extensively tested, but the goal (as I see it) is to make BOLT IV a functioning
* bike that functions via fly-by-wire (electronic transmission of throttle signal), but
* no traction control because that will take much more research and testing. This will
* be raw throttle in to raw throttle out. The things that I can see will need to be
* done will be marked by @todo.
*
*******************************************************************************
* Notes:
*  Follow the GNOME style guide
*
*  Anything related to individual peripherals should be in corresponding .c and .h files
*      that are then included below
*
*******************************************************************************/

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
void getIMUdata();

void main(void)
{
    init();
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

        // @todo - implement ADC and write getThrottle() method. Verify values with separate ADC and ensure full range of input works
        //         This could probably be done by extending Quinton's ADC code in the position_sensor branch
        float rawThrottleIn = 3; // getThrottle()

        torque_request = rawThrottleIn; // unmodified

        /* No TC, unnecessary
        int rawFrontSusp = 0; // getFrontSusp()
        int rawBackSusp = 0; // getBackSusp()
        *
        *
        * Follow lookup table logic
        * Specific logic TBD
        *
        * The basic structure will be as follows:
        *      First, check which TC categories need to be calculated/looked up
        *      Then, do those calculations and store the results
        *      Look at all the results and find the largest reduction
        *      Move forward with that
        *
        *
        *bool checks[4]; // raw_in, traction_control, anti_wheelie, anti_jerk
        *
        *checks[0] = true; // raw_in will always qualify
        *
        *
        * Carry out any calculations
        */


        // Send torque request to motor - this will be the raw torque request, unmodified
//        requestTorque(3.00); // @todo - verify functionality in DAC file over full range of output values, Stephen wrote this and it's merged
        float voltage = getADCRawValue() / 4096.0 * 3.0;
        setDACOutputVoltage(voltage);

        // Send a test CANmsg
        /* @todo - you'll still want CAN messages out of this so that the CANlogger can pick
         * them up and you can record and verify that IO throttle matches up. See can_etcm.h
         * and can_etcm.c, written by Tyler. For any further functionality, implement from
         * the CAN examples. They can be finicky and you should make sure there aren't any
         * jumpers or other hardware on the silkscreen that need to be arranged in a certain
         * way, but the examples will work. Also, consider buying a CAN analyzer.
        //CANtest()
         * @todo - verify test works, then put in actual data
         */

        // Flash the blue LED - this is just debug/verify the board is actually functioning
//        LEDflash();

//        getIMUdata(); // @todo - this is Quinton's code, I'm fairly sure that it works, still verify

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
    Interrupt_initModule();
    Interrupt_initVectorTable();

    //initLookup(); // removed, no lookup table
    initADC();
    //initEPWM();   // should be removed, only TC-required
    initADCSOC(); // @todo - honestly I don't remember what this was for. Aux bat SOC is measured
                    // on MSP432 (and should be rewritten), check on PCB. Check with Quinton if you
                    // can't figure it out

    EINT;
    ERTM;

    initCAN();
    //initSCI();    // @todo - figure out which or both initSCI methods are necessary
    initSCIFIFO();
    initDAC();
}

//Initialize lookup tables (should be unused)
/*
 *  void initLookup(void)
 * {
 *   // Open file containing tables
 *
 *   // Load tables into ROM
 * }



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
            //CANA_transmitMsg(dataBuffer, 8, 1);
            //CANA_transmitMsg(dataBuffer+8, 8, 2);   // increment pointer
            //CANA_transmitMsg(dataBuffer+16, 2, 3);   // increment pointer
            return;
        }
        // loop again
    }


}

//
// End of File
//
