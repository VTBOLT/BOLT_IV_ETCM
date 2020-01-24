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
#include <IMU.h>
#include <leds_etcm.h>

//****************
// Defines
//****************
#define GPIO_CFG_SYNC_IN GPIO_67_GPIO67
#define GPIO_SYNC_IN 67
#define IMU_FRAME_SIZE 18

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
 //   int torque_request = 0; // likely to change type
    // start the timer
 //   startTimer0();
    while (1)
    {
        // Pull in sensor data to local variables
        // This will use getters inside the peripheral .h/.c files

        // Follow lookup table logic
        // Specific logic TBD

        // Carry out any calculations

        // Send torque request to motor
 //       requestTorque(torque_request);

        // Send a test CANmsg
        //CANtest();

        // Flash the blue LED
        LEDflash();

        // uart test
        //SCItest();

        //getIMUdata();
        displayIMU_CAN();

        // after 5 seconds, reduce period to 500mS
        //    if (cpuTimer0IntCount >= 5){
          //      reloadTimer0(500);
            //}


    }
}

//Initialize, runs all initialization functions
void init(void)
{
    // Initialize device clock and peripherals
    Device_init();
    initGPIO();     // do not move
    initLEDS();

    //initLookup();
    //initADC();
    //initEPWM();
    //initADCSOC();
    //initCAN();
  //  initTimer0();
    //initSCI();
    initSCIwithFIFO();
    initInterrupts();
}

//Initialize lookup tables
void initLookup(void)
{
    // Open file containing tables
    // Load tables into ROM
}

/**
 * Send out a test message over CAN
 * ID: 0x401

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
//    CANA_transmitMsg(msg, 4, 1);
}
*/

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
    GPIO_writePin(GPIO_SYNC_IN, 1);
    // reset SYNC_IN GPIO
    GPIO_writePin(GPIO_SYNC_IN, 0);

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
 //           CANA_transmitMsg(dataBuffer, 8, 1);
   //         CANA_transmitMsg(dataBuffer+8, 8, 2);   // increment pointer
     //       CANA_transmitMsg(dataBuffer+16, 2, 3);   // increment pointer
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

}

void displayIMU_CAN(void){
    // fetch new IMU data frame
    updateIMUbuffer();

    // wait for new frame
    while (!IMUframeRcvd);

    // put data on CAN bus
//    CANA_transmitMsg(IMUdataBuffer, 8, 1);
  //  CANA_transmitMsg(IMUdataBuffer + 8, 8, 2);   // increment pointer
    //CANA_transmitMsg(IMUdataBuffer + 16, 2, 3);   // increment pointer

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
    uint8_t FIFOdebug = HWREGH(SCI_BASE + SCI_O_FFRX);
    uint8_t FIFOsize = SCI_getRxFIFOStatus(SCI_BASE);

    // get the data from the FIFO
    uint8_t dataIndex = 0;
    static uint8_t dataIndexPrev = 0;   // used to append data to IMU buffer

    if ((dataIndexPrev + FIFOsize) > IMU_FRAME_SIZE){
        // error
        dataIndexPrev = 0;
        return;
    }

    for (; dataIndex < FIFOsize; dataIndex++){
        IMUdataBuffer[dataIndex + dataIndexPrev] = SCI_readCharNonBlocking(SCI_BASE);
    }
    dataIndexPrev = dataIndexPrev + dataIndex;
    // complete packet received
    if (dataIndexPrev >= (IMU_FRAME_SIZE)){
        IMUframeRcvd = true;
        dataIndexPrev = 0;
    }
    uint16_t temp[4];

    float pitch;
    float roll;

    temp[0] = IMUdataBuffer[8];
    temp[1] = IMUdataBuffer[9];
    temp[2] = IMUdataBuffer[10];
    temp[3] = IMUdataBuffer[11];
    memcpy(&pitch, &temp, 4);

    temp[0] = IMUdataBuffer[12];
    temp[1] = IMUdataBuffer[13];
    temp[2] = IMUdataBuffer[14];
    temp[3] = IMUdataBuffer[15];
    memcpy(&roll, &temp, 4);
}

//void getIMUdataINT(void){
//    if (IMUframeRcvd){
//        // something is wrong, should not be here
//        // clear buffer and return
//        SCI_resetRxFIFO(SCI_BASE);
//        //uint8_t FIFOsize = SCI_getRxFIFOStatus(SCI_BASE);
//        //FIFOsize = SCI_getRxFIFOStatus(SCI_BASE);
//        return;
//    }
//
//    // get current amount of data in FIFO
//    uint8_t FIFOdebug = HWREGH(SCI_BASE + SCI_O_FFRX);
//    uint8_t FIFOsize = SCI_getRxFIFOStatus(SCI_BASE);
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
//        SCI_BASE);
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
//    initTimer0Interrupt();

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
    SCI_enableInterrupt(SCI_BASE, SCI_INT_RXFF);
    SCI_disableInterrupt(SCI_BASE, (SCI_INT_RXERR | SCI_INT_TXFF));

    // set the level at which the FIFO_RX flag is thrown
    SCI_setFIFOInterruptLevel(SCI_BASE, SCI_FIFO_TX2, SCI_FIFO_RX1);
    SCI_performSoftwareReset(SCI_BASE);

    // clear buffer
    SCI_resetRxFIFO(SCI_BASE);

    // enable SCI_RX PIE interrupt
    Interrupt_enable(INT_SCIB_RX);

    // clear PIEACK
    // see table 3-2 on pg.102 in tech. ref. manual for groups
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP8);
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
    // SCI_resetRxFIFO(SCI_BASE);

    SCI_clearOverflowStatus(SCI_BASE);
    SCI_clearInterruptStatus(SCI_BASE, SCI_INT_RXFF);

    // Issue PIE ack
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);

}

//
// End of File
//
