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
#include "device.h"
#include "adc_etcm.h"
#include "dac_etcm.h"
#include "device.h"
#include "CPUTimer_etcm.h"
#include "uart_etcm.h"
#include "gpio_etcm.h"
#include "interrupt_etcm.h"
#include "epwm_etcm.h"
#include <stdlib.h>
#include "string.h"

typedef enum {INPUT, OUTPUT} testState;

//Globals
int CPUTimer0Count = 0;
char outButtonStatus = 0x0000;
char resetButtonStatus = 0x0000;

//Function Prototypes.
void init(void);
int myPow(int base, int c);
void run(void);
void initLookup(void);
__interrupt void cpuTimer0ISR(void);
void getInput(char* prompt, char* in, unsigned int length);
bool Timer0Expired();

void main(void)
{
    init();
    run();

    return;
}

void run(void)
{
    char read[4] = {'0','0','0','\0'};
    char write[11] = {'0','0','0','0','0','0','0','0','0','0','\n'};
    while (1)
    {
        //Resetting Values
        int suspensionTravel[2] = {0,0};
        int pSwitches[3] = {0,0,0};
        int bSwitches[2] = {0,0};
        int wSensors[2] = {0,0};
        int tSwitch = 0;

        //Read in values from serial
        SCIread(SCI_DEBUG_BASE, (uint16_t *) read, 3); //Front Wheel Speed
        wSensors[0] = atoi(read);
        SCIread(SCI_DEBUG_BASE, (uint16_t *) read, 3); //Rear Wheel Speed
        wSensors[1] = atoi(read);
        SCIread(SCI_DEBUG_BASE, (uint16_t *) read, 3); //Front Susp Travel
        suspensionTravel[0] = atoi(read);
        SCIread(SCI_DEBUG_BASE, (uint16_t *) read, 3); //Rear Susp Travel
        suspensionTravel[1] = atoi(read);

        char oneBit[4] = {'0','0','0','\0'};;
        SCIread(SCI_DEBUG_BASE, (uint16_t *) oneBit, 3); //Front Brake
        bSwitches[0] = atoi(oneBit);
        SCIread(SCI_DEBUG_BASE, (uint16_t *) oneBit, 3); //Back Brake
        bSwitches[1] = atoi(oneBit);
        SCIread(SCI_DEBUG_BASE, (uint16_t *) oneBit, 3); //Profile 1
        pSwitches[0] = atoi(oneBit);
        SCIread(SCI_DEBUG_BASE, (uint16_t *) oneBit, 3); //Profile 2
        pSwitches[1] = atoi(oneBit);
        SCIread(SCI_DEBUG_BASE, (uint16_t *) oneBit, 3); //Profile 3
        pSwitches[2] = atoi(oneBit);
        SCIread(SCI_DEBUG_BASE, (uint16_t *) oneBit, 3); //Throttle
        tSwitch = atoi(oneBit);

        //Set Values on Pins
        GPIO_writePin(67, pSwitches[0]); //Profile Switches (J1 5,6,7)
        GPIO_writePin(111, pSwitches[1]);
        GPIO_writePin(60, pSwitches[2]);

        GPIO_writePin(61, bSwitches[0]); //Brake Switches (J2 19, 18)
        GPIO_writePin(123, bSwitches[1]);

        GPIO_writePin(122, tSwitch); //Throttle Closed Switch (J2 17)

        setDACOutputVoltage(DACA_BASE, (150 - suspensionTravel[0]) * (3.0/150)); //Suspension Travel (J3 30)
        setDACOutputVoltage(DACB_BASE, (150 - suspensionTravel[1]) * (3.0/150)); //Suspension Travel (J7 70)

        //Calculate PWM Signal
        initEPWM1(2*(wSensors[0]*4*40)); //Front Wheel Speed (J4 40)
        initEPWM2(2*(wSensors[1]*4*40)); //Rear Wheel Speed  (J4 38)

        uint16_t throttle_input = getADCVal(ADCB_BASE, ADCBRESULT_BASE, ADC_SOC_NUMBER1, ADC_SOC_NUMBER2, ADC_INT_NUMBER1); //(J3 25)
        uint16_t motor_request = getADCVal(ADCA_BASE, ADCARESULT_BASE, ADC_SOC_NUMBER1, ADC_SOC_NUMBER2, ADC_INT_NUMBER1); //(J3 29)

        unsigned int c = 0;
        while (c < 10)
        {
            write[9 - c] = (throttle_input % 10) + 48;
            throttle_input /= 10;
            c++;
        }
        SCIwrite(SCI_DEBUG_BASE, (uint16_t *) write, strlen(write));

        c = 0;
        while (c < 10)
        {
            write[9 - c] = (motor_request % 10) + 48;
            motor_request /= 10;
            c++;
        }
        SCIwrite(SCI_DEBUG_BASE, (uint16_t *) write, strlen(write));
        /* convert ADC val to char */

    }
}
int myPow(int base, int c)
{
    unsigned int a = 0;
    for (a = 1; a < c; a++)
    {
        base *= base;
    }
    return base;
}
//Initialize, runs all initialization functions
void init(void)
{
    Device_init();
    Device_initGPIO();
    initInterrupt();
    initLookup();

    initGPIO();

    initADC(ADCA_BASE);
    initADC(ADCB_BASE);
    initADCSOC(ADCA_BASE, ADC_CH_ADCIN2, ADC_SOC_NUMBER1, ADC_SOC_NUMBER2, ADC_INT_NUMBER1);
    initADCSOC(ADCB_BASE, ADC_CH_ADCIN3, ADC_SOC_NUMBER1, ADC_SOC_NUMBER2, ADC_INT_NUMBER1);

    initSCI();

    initDAC(DACA_BASE);
    initDAC(DACB_BASE);

    addInterrupt(&cpuTimer0ISR, INT_TIMER0);
    initTimer(CPUTIMER0_BASE, 5000, 0);
    enableInterrupts();
    startTimer(CPUTIMER0_BASE);
    EINT;
    ERTM;
}
//Initialize lookup tables
void initLookup(void)
{
    // Open file containing tables

    // Load tables into ROM
}

__interrupt void cpuTimer0ISR(void)
{
    CPUTimer0Count++;
    sampleGPIO(&outButtonStatus);
    sampleGPIO(&resetButtonStatus);
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP1);
}

void getInput(char* prompt, char* in, unsigned int length){
    char* newline = "\r\n";
    SCIwrite(SCI_DEBUG_BASE, (uint16_t *) prompt, strlen(prompt));
    SCIwrite(SCI_DEBUG_BASE, (uint16_t *) in, length);
    SCIwrite(SCI_DEBUG_BASE, (uint16_t *) newline, strlen(newline));
}


//
// End of File
//
