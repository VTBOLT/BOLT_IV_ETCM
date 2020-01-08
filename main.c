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


//Function Prototypes.
void init(void);
void run(void);
void initLookup(void);
void CANtest(void);

void main(void)
{
    init();
    run();

    return;
}

void run(void)
{
    int torque_request = 0; // likely to change type

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
        CANtest();
    }
}

//Initialize, runs all initialization functions
void init(void)
{
    initLookup();
    initADC();
    initEPWM();
    initADCSOC();
    initCAN();
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
 * TODO: Check pins
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
    CANA_transmitMsg1(msg, 8);
}



//
// End of File
//
