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
#include <ecap_etcm.h>

#include "string.h"     // efficiency?

//****************
// Defines
//****************
#define GPIO_CFG_SYNC_IN GPIO_67_GPIO67
#define GPIO_SYNC_IN 67
#define IMU_FRAME_SIZE 18

//***********
// Typedefs, ...
//***********
typedef struct SensorData
{
    float wheelSpeedFront;
    float wheelSpeedRear;
    float throttlePosition;
    uint16_t throttlePositionRaw;

}vehicleSensorData_t;

// Strings
const char* ETCM_START = "ETCM_START\r\n";
const char* THROTTLE_POS = "Throttle Position: ";
const char* WHEEL_SPEED = "Wheel Speed: ";

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
extern void sendCAN(void);
void getVehicleSensorData(vehicleSensorData_t *vehicleSensorData);
void sendCAN_nonINT(vehicleSensorData_t vehicleSensorData);
void make_5digit_NumString(unsigned int num, uint16_t *string);
void debugPrint(vehicleSensorData_t vehicleSensorData);



//**********
// Globals
//**********
volatile uint8_t IMUdataBuffer[IMU_FRAME_SIZE]; // 18 byte frames
volatile bool IMUframeRcvd = false;



void main(void)
{
    init();
    run();

    return;
}

void run(void)
{
    // container for vehicle sensor data
    vehicleSensorData_t vehicleSensorData = {0,0,0,0};

    int torque_request = 0; // likely to change type
    uint16_t throttleADC = 0;

    // start the timer
    startTimer0();
    while (1)
    {
        // Get vehicle sensor current values
        getVehicleSensorData(&vehicleSensorData);

        // Send that data over CAN for debug
        sendCAN_nonINT(vehicleSensorData);

        // Send a test CANmsg
        //CANtest();

        // Flash the blue LED
        LEDflash();

        debugPrint(vehicleSensorData);



        // uart test
        //SCItest();

        //getIMUdata();
        //displayIMU_CAN();

        // grab a throttle ADC value
        //throttleADC = getThrottleADC();


    }
}

//Initialize, runs all initialization functions
void init(void)
{
    // Initialize device clock and peripherals
    Device_init();
    initGPIO();     // do not move
    initLEDS();

    initADC();

    initCAN();
    initTimer0();
    initSCI();
    initECAP();

    initInterrupts();
}

void make_5digit_NumString(unsigned int num, uint16_t *string)
{
    string[0]= (num  / 10000) +'0';
    string[1]= ((num%10000) / 1000) +'0';
    string[2]= ((num%1000) / 100) +'0';
    string[3]= ((num%100) / 10) +'0';
    string[4]= ((num%10) / 1) +'0';
}

void debugPrint(vehicleSensorData_t vehicleSensorData){
    uint16_t throttleString[5];
    uint16_t freqString[5];

    // convert throttle ADC value to ASCII
    make_5digit_NumString(vehicleSensorData.throttlePositionRaw, throttleString);

    // convert wheel speed freq to ASCII
    uint16_t wheelSpeedFreqInt = (signalFreq*100);
    make_5digit_NumString(wheelSpeedFreqInt, freqString);

    // return cursor to top
    SCIwrite(SCI_DEBUG_BASE, "\033[2J", 8);      // clear
    SCIwrite(SCI_DEBUG_BASE, "\033[0;0H", 8);    // home
    SCIwrite(SCI_DEBUG_BASE, "\033[?25l", 8);    // hide cursor

    // print data
    SCIwrite(SCI_DEBUG_BASE, ETCM_START, strlen(ETCM_START));
    SCIwrite(SCI_DEBUG_BASE, THROTTLE_POS, strlen(THROTTLE_POS));
    SCIwrite(SCI_DEBUG_BASE, throttleString, 5);

    SCIwrite(SCI_DEBUG_BASE, "\r\n", strlen("\r\n"));
    SCIwrite(SCI_DEBUG_BASE, WHEEL_SPEED, strlen(WHEEL_SPEED));
    SCIwrite(SCI_DEBUG_BASE, freqString, 5);


}

//Initialize lookup tables
void initLookup(void)
{
    // Open file containing tables
    // Load tables into ROM
}


void getVehicleSensorData(vehicleSensorData_t *vehicleSensorData){
    vehicleSensorData->throttlePositionRaw = getThrottleADC();
    vehicleSensorData->throttlePosition = getThrottlePercent(vehicleSensorData->throttlePositionRaw);
}


/**
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
    Device_initGPIO();      // must be called first?

    //********
    // GPIOs
    //--------

    // SYNC_IN
    GPIO_setPinConfig(GPIO_CFG_SYNC_IN);
    GPIO_setPadConfig(GPIO_SYNC_IN, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(GPIO_SYNC_IN, GPIO_DIR_MODE_OUT);
    GPIO_writePin(GPIO_SYNC_IN, 0); // default state

    //********

}

/**
 * getIMUdata(void) grabs an 18 byte IMU frame from the SCI_RX buffer
 * and puts it out over CAN.
 *
 * No CPU interrupts are used.
 */

void getIMUdata(){
    // data container
    volatile uint8_t dataBuffer[IMU_FRAME_SIZE]; // 18 byte frames
    volatile bool IMUframeRcvd = false;

    // make sure FIFO is enabled
    if (!SCI_isFIFOEnabled(SCI_IMU_BASE)){
        // return error
        return;
    }
    // clear RX_FIFO
    SCI_resetRxFIFO(SCI_IMU_BASE);

    // configure level at which INT flag is thrown
    // RX1 = 1 byte in buffer
    SCI_setFIFOInterruptLevel(SCI_IMU_BASE, SCI_FIFO_TX1, SCI_FIFO_RX1);

    // enable RX_FIFO INT
    SCI_enableInterrupt(SCI_IMU_BASE, SCI_INT_RXFF);

    // strobe SYNC_IN GPIO
    GPIO_writePin(GPIO_SYNC_IN, 1);
    // reset SYNC_IN GPIO
    GPIO_writePin(GPIO_SYNC_IN, 0);

    // wait for buffer fill (INT flag)
    while((HWREGH(SCI_IMU_BASE + SCI_O_FFRX) & SCI_FFRX_RXFFINT) != SCI_FFRX_RXFFINT);

    // get data one byte at a time
    volatile uint8_t dataIndex = 0;
    while(!IMUframeRcvd){
        // check buffer
        while(SCI_getRxFIFOStatus(SCI_IMU_BASE) == SCI_FIFO_RX0);    // wait for data (will get stuck here)
        // grab byte
        dataBuffer[dataIndex] = SCI_readCharNonBlocking(SCI_IMU_BASE);
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

/**
 * This will start the IMU buffer update process.
 * Interrupt handled.
 */
void updateIMUbuffer(void){
    // clear data buffer and flag
    IMUframeRcvd = false;
    uint8_t dataIndex = 0;
    for(; dataIndex < IMU_FRAME_SIZE; dataIndex++){
        IMUdataBuffer[dataIndex] = 0;
    }
    // strobe SYNC_IN GPIO
    GPIO_writePin(GPIO_SYNC_IN, 1);
    // delay? (pulse width)
    // reset SYNC_IN GPIO
    GPIO_writePin(GPIO_SYNC_IN, 0);

    // buffer will start to fill as interrupt happens
}

void displayIMU_CAN(void){
    // fetch new IMU data frame
    updateIMUbuffer();

    // wait for new frame
    while (!IMUframeRcvd);

    // data is sent out over CAN via a timer INT

}

/**
 * SCI_FIFO_RX will throw an interrupt every time there is at least
 * one byte in the buffer. This function is called by the SCI_ISR()
 * to fetch the data and store it in a global container.
 *
 * TODO: Verify frame integrity (start-of-header, checksum)
 */
void getIMUdataINT(void){
    // get current amount of data in FIFO
    uint8_t FIFOdebug = HWREGH(SCI_IMU_BASE + SCI_O_FFRX);
    uint8_t FIFOsize = SCI_getRxFIFOStatus(SCI_IMU_BASE);

    // get the data from the FIFO
    uint8_t dataIndex = 0;
    static uint8_t dataIndexPrev = 0;   // used to append data to IMU buffer

    if ((dataIndexPrev + FIFOsize) > IMU_FRAME_SIZE){
        // error
        dataIndexPrev = 0;
        return;
    }

    for (; dataIndex < FIFOsize; dataIndex++){
        IMUdataBuffer[dataIndex + dataIndexPrev] = SCI_readCharNonBlocking(SCI_IMU_BASE);
    }
    dataIndexPrev = dataIndexPrev + dataIndex;
    // complete packet received
    if (dataIndexPrev >= (IMU_FRAME_SIZE)){
        IMUframeRcvd = true;
        dataIndexPrev = 0;
    }
}

//void getIMUdataINT(void){
//    if (IMUframeRcvd){
//        // something is wrong, should not be here
//        // clear buffer and return
//        SCI_resetRxFIFO(SCI_IMU_BASE);
//        //uint8_t FIFOsize = SCI_getRxFIFOStatus(SCI_IMU_BASE);
//        //FIFOsize = SCI_getRxFIFOStatus(SCI_IMU_BASE);
//        return;
//    }
//
//    // get current amount of data in FIFO
//    uint8_t FIFOdebug = HWREGH(SCI_IMU_BASE + SCI_O_FFRX);
//    uint8_t FIFOsize = SCI_getRxFIFOStatus(SCI_IMU_BASE);
//
//    // get the data from the FIFO
//    uint8_t dataIndex = 0;
//    static uint8_t dataIndexPrev = 0;   // used to append data to IMU buffer
//
//    if ((dataIndexPrev + FIFOsize) > IMU_FRAME_SIZE){
//        // error
//        dataIndexPrev = 0;
//        return;
//    }
//
//    for (; dataIndex < FIFOsize; dataIndex++){
//        IMUdataBuffer[dataIndex + dataIndexPrev] = SCI_readCharNonBlocking(
//        SCI_IMU_BASE);
//    }
//    dataIndexPrev = dataIndexPrev + dataIndex;
//    if (dataIndexPrev >= (IMU_FRAME_SIZE)){
//        IMUframeRcvd = true;
//        dataIndexPrev = 0;
//    }
//}

void initInterrupts(void){
    // Initialize PIE and clear PIE registers. Disables CPU interrupts.
    Interrupt_initModule();
    // Initialize the PIE vector table with pointers to the shell Interrupt
    Interrupt_initVectorTable();


    //*******************************
    // Interrupt init calls go here
    //-------------------------------
    initIMUinterrupt();
    initTimer0Interrupt();

    Interrupt_register(INT_ECAP1, &ecap1ISR);
    Interrupt_enable(INT_ECAP1);
    //*******************************

    // enable CPU interrupts
    // this should always be last
    // see Pg. 100 of Tech Ref Manual
    Interrupt_enableMaster();
    //EINT;
    //ERTM;
}


void initIMUinterrupt(void){
    // set SCI RX interrupt handler (ISR) in vector table
    Interrupt_register(INT_SCIB_RX, SCI_ISR);

    // enable SCI FIFO interrupts
    SCI_enableInterrupt(SCI_IMU_BASE, SCI_INT_RXFF);
    SCI_disableInterrupt(SCI_IMU_BASE, (SCI_INT_RXERR | SCI_INT_TXFF));

    // set the level at which the FIFO_RX flag is thrown
    SCI_setFIFOInterruptLevel(SCI_IMU_BASE, SCI_FIFO_TX2, SCI_FIFO_RX1);
    SCI_performSoftwareReset(SCI_IMU_BASE);

    // clear buffer
    SCI_resetRxFIFO(SCI_IMU_BASE);

    // enable SCI_RX PIE interrupt
    Interrupt_enable(INT_SCIB_RX);

    // clear PIEACK
    // see table 3-2 on pg.102 in tech. ref. manual for groups
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);
}



/**
 * SCI_ISR(void) is the interrupt handler for SCI_RX CPU interrupt.
 * Registered in vector table by initIMUinterrupt()
 *
 * Interrupt fires when a data byte has been received in the
 * SCI RX FIFO buffer.
 */
__interrupt void SCI_ISR(void){

    //*****************
    // do stuff here
    //-----------------
    toggleRedLED();
    getIMUdataINT();
    //****************

    // clear buffer to prevent another interrupt
    // Do not do this. FIFO is automatically cleared as data is removed.
    // SCI_resetRxFIFO(SCI_IMU_BASE);

    SCI_clearOverflowStatus(SCI_IMU_BASE);
    SCI_clearInterruptStatus(SCI_IMU_BASE, SCI_INT_RXFF);

    // Issue PIE ack
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);

}


void sendCAN_nonINT(vehicleSensorData_t vehicleSensorData){
    uint16_t dataMSG[2];
    dataMSG[0] = vehicleSensorData.throttlePositionRaw;
    dataMSG[1] = (uint16_t)vehicleSensorData.wheelSpeedFront;

    CANA_transmitMsg(dataMSG, 2, 1);

}

void sendCAN(void){
    // put IMU data
    //CANA_transmitMsg(IMUdataBuffer, 8, 1);
    //CANA_transmitMsg(IMUdataBuffer + 8, 8, 2);   // increment pointer
    //CANA_transmitMsg(IMUdataBuffer + 16, 2, 3);   // increment pointer

    // half assed typecasting
    uint16_t signalFreqInteger = (uint16_t)signalFreq;
    // put wheel speed (ecap)
    CANA_transmitMsg(&signalFreqInteger, 1, 4);     // will truncate to integer!
}

//
// End of File
//
