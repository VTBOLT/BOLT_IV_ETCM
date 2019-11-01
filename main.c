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
#include "adc_etcm.h"

//Function Prototypes.
void init(void);
void run(void);
void initLookup(void);
void requestTorque(int torque);

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
    }
}

//Initialize, runs all initialization functions
void init(void)
{
    initLookup();
    initADC();
    initEPWM();
    initADCSOC();
}

//Initialize lookup tables
void initLookup(void)
{
    // Open file containing tables

    // Load tables into ROM
}



//
// End of File
//
