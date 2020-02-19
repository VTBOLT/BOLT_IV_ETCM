#include "IMU.h"


//Globals
volatile float IMU_yaw;
volatile float IMU_pitch;
volatile float IMU_roll;

volatile uint8_t IMUdataBuffer[IMU_FRAME_SIZE]; // 18 byte frames
volatile bool IMUframeRcvd = false;

/**
 * This will start the IMU buffer update process.
 * Interrupt handled.
 */
void strobeIMUSyncIn(void)
{
    // clear data buffer and flag
    IMUframeRcvd = false;
    uint8_t dataIndex = 0;
    for (; dataIndex < IMU_FRAME_SIZE; dataIndex++)
    {
        IMUdataBuffer[dataIndex] = 0;
    }

    // strobe SYNC_IN GPIO
    GPIO_writePin(GPIO_SYNC_IN, 1);
    // reset SYNC_IN GPIO
    GPIO_writePin(GPIO_SYNC_IN, 0);
    // buffer will start to fill as interrupt happens
}


float getIMUYaw(void)
{
    return IMU_yaw;
}

float getIMUPitch(void)
{
    return IMU_pitch;
}

float getIMURoll(void)
{
    return IMU_roll;
}

/**
 * Module GPIO inits are in their respective .c file.
 */
void initIMUSyncIn(void)
{
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

void getIMUdataINT(void)
{
// get current amount of data in FIFO
    uint8_t FIFOdebug = HWREGH(SCI_IMU_BASE + SCI_O_FFRX);
    uint8_t FIFOsize = SCI_getRxFIFOStatus(SCI_IMU_BASE);

// get the data from the FIFO
    uint8_t dataIndex = 0;
    static uint8_t dataIndexPrev = 0;   // used to append data to IMU buffer

    if ((dataIndexPrev + FIFOsize) > IMU_FRAME_SIZE)
    {
        // error
        dataIndexPrev = 0;
        return;
    }

    for (; dataIndex < FIFOsize; dataIndex++)
    {
        IMUdataBuffer[dataIndex + dataIndexPrev] = SCI_readCharNonBlocking(
        SCI_IMU_BASE);
    }
    dataIndexPrev = dataIndexPrev + dataIndex;
// complete packet received
    if (dataIndexPrev >= (IMU_FRAME_SIZE))
    {
        IMUframeRcvd = true;
        dataIndexPrev = 0;
    }

    char temp_buffer[4];

    temp_buffer[3] = IMUdataBuffer[4];
    temp_buffer[2] = IMUdataBuffer[5];
    temp_buffer[1] = IMUdataBuffer[6];
    temp_buffer[0] = IMUdataBuffer[7];

//convert the temporary buffer to a float
    IMU_yaw = *(float *) &temp_buffer;

    temp_buffer[3] = IMUdataBuffer[8];
    temp_buffer[2] = IMUdataBuffer[9];
    temp_buffer[1] = IMUdataBuffer[10];
    temp_buffer[0] = IMUdataBuffer[11];

    IMU_pitch = *(float *) &temp_buffer;

    temp_buffer[3] = IMUdataBuffer[12];
    temp_buffer[2] = IMUdataBuffer[13];
    temp_buffer[1] = IMUdataBuffer[14];
    temp_buffer[0] = IMUdataBuffer[15];

    IMU_roll = *(float *) &temp_buffer;

}

/**
 * SCI_ISR(void) is the interrupt handler for SCI_RX CPU interrupt.
 * Registered in vector table by initIMUinterrupt()
 *
 * Interrupt fires when a data byte has been received in the
 * SCI RX FIFO buffer.
 */
__interrupt void SCI_ISR(void)
{

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

void initIMUinterrupt(void)
{
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

    IMU_yaw = 0;
    IMU_pitch = 0;
    IMU_roll = 0;
}

/**
 * initSCIwithFIFO(void) initializes the SCI module for use with
 * the FIFO buffer.
 */
void initIMUTransfer(void)
{
    // GPIO TX
    GPIO_setMasterCore(GPIO_SCITX_IMU, GPIO_CORE_CPU1);
    GPIO_setPinConfig (GPIO_CFG_SCITX_IMU);
    GPIO_setPadConfig(GPIO_SCITX_IMU, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(GPIO_SCITX_IMU, GPIO_DIR_MODE_OUT);
    GPIO_setQualificationMode(GPIO_SCITX_IMU, GPIO_QUAL_ASYNC);

    //GPIO RX
    GPIO_setMasterCore(GPIO_SCIRX_IMU, GPIO_CORE_CPU1);
    GPIO_setPinConfig (GPIO_CFG_SCIRX_IMU);
    GPIO_setPadConfig(GPIO_SCIRX_IMU, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(GPIO_SCIRX_IMU, GPIO_DIR_MODE_IN);
    GPIO_setQualificationMode(GPIO_SCIRX_IMU, GPIO_QUAL_ASYNC);

    // configure module
    // 8N1
    SCI_setConfig(SCI_IMU_BASE, DEVICE_LSPCLK_FREQ, SCI_IMU_BAUD, (SCI_CONFIG_WLEN_8 |SCI_CONFIG_STOP_ONE |SCI_CONFIG_PAR_NONE));
    SCI_enableModule(SCI_IMU_BASE);

    SCI_resetChannels(SCI_IMU_BASE);

    SCI_enableFIFO(SCI_IMU_BASE);
}
