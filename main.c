/**
 * main.c
 */

#include "driverlib.h"
#include "device.h"

//Function Prototypes.
void init(void);
void run(void);
void initLookup(void);
void initADC(void);
void initEPWM(void);
void initADCSOC(void);
void requestTorque(int torque);
__interrupt void adcA0ISR(void);

//Main, calls init and run
int main(void)
{
    init();
    run();
    return 0;
}

//Initialize, runs all initialization functions
void init(void)
{
    initLookUp();
    initADC();
    initEPWM();
    initADCSOC();
}

//Main loop
void run(void)
{
    int torque_request;
    while(true)
    {
        torque_request = 0;
        //check sensors

        //check lookup table

        //do any neccesary calculations

        //send torque request to motor
        requestTorque(torque_request);
    }
}

//Initialize lookup tables
void initLookup(void)
{
    //open file containing tables

    //load tables into rom
}

//Initialize and power up the ADC
void initADC(void)
{
    //initialize the ADC.
}



//send torque request to motor controller
void requestTorque(int torque)
{

}
