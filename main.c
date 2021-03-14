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
#include <math.h>

typedef enum
{
    INPUT, OUTPUT
} testState;

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
    char read[9] = { '0', '0', '0', '0', '0', '0', '0', '0', '\0' };
    char write[11] = { '0', '0', '0', '0', '0', '0', '0', '0', '0', '0', '\n' };
    while (1)
    {
        //Resetting Values
        float ypr[3] = { 0.0, 0.0, 0.0 };

        SCIread(SCI_DEBUG_BASE, (uint16_t *) read, 9); //Yaw
        ypr[0] = atof(read) + .00001;
        SCIread(SCI_DEBUG_BASE, (uint16_t *) read, 9); //Pitch
        ypr[1] = atof(read) + .00001;
        SCIread(SCI_DEBUG_BASE, (uint16_t *) read, 9); //Roll
        ypr[2] = atof(read) + .00001;;

        // for testing purposes... these will need to be set by the Kalman Filter functions
        float output_yaw = ypr[0];
        float output_pitch = ypr[1];
        float output_roll = ypr[2];

        // redundant, but easier to loop through
        float output_ypr[] = { output_yaw, output_pitch, output_roll };

        // assumed precision of the float... places past decimal point
        int precision = 4;

        int i;
        int j;

        for (i = 0; i < 3; i++)
        {
            long temp_value = (long)(output_ypr[i] * powf(10.0, precision));
            for (j = 0; j < precision; j++)
            {
                write[9 - j] = (char) ((temp_value % 10) + 48);
                temp_value /= 10;
            }
            write[9 - precision] = '.';
            for (j = precision + 1; j < 10; j++)
            {
                write[9 - j] = (char) ((temp_value % 10) + 48);
                temp_value /= 10;
            }
            SCIwrite(SCI_DEBUG_BASE, (uint16_t *) write, strlen(write));
        }
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
    initADCSOC(ADCA_BASE, ADC_CH_ADCIN2, ADC_SOC_NUMBER1, ADC_SOC_NUMBER2,
               ADC_INT_NUMBER1);
    initADCSOC(ADCB_BASE, ADC_CH_ADCIN3, ADC_SOC_NUMBER1, ADC_SOC_NUMBER2,
               ADC_INT_NUMBER1);

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

void getInput(char* prompt, char* in, unsigned int length)
{
    char* newline = "\r\n";
    SCIwrite(SCI_DEBUG_BASE, (uint16_t *) prompt, strlen(prompt));
    SCIwrite(SCI_DEBUG_BASE, (uint16_t *) in, length);
    SCIwrite(SCI_DEBUG_BASE, (uint16_t *) newline, strlen(newline));
}

//
// End of File
//
