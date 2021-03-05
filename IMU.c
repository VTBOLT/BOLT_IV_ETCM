#include "IMU.h"

//Globals
volatile float IMU_yaw;
volatile float IMU_pitch;
volatile float IMU_roll;

volatile float IMU_yaw_measured;
volatile float IMU_pitch_measured;
volatile float IMU_roll_measured;

volatile float estimateError_yaw;
volatile float measurementError_yaw;
volatile float estimateError_pitch;
volatile float measurementError_pitch;
volatile float estimateError_roll;
volatile float measurementError_roll;

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

float getIMUYawM(void)
{
    return IMU_yaw_measured;
}

float getIMUPitchM(void)
{
    return IMU_pitch_measured;
}

float getIMURollM(void)
{
    return IMU_roll_measured;
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

    uint32_t temp_buffer;
    float temp_float;
    temp_buffer = IMUdataBuffer[7];
    temp_buffer = temp_buffer << 8;
    temp_buffer = temp_buffer | IMUdataBuffer[6];
    temp_buffer = temp_buffer << 8;
    temp_buffer = temp_buffer | IMUdataBuffer[5];
    temp_buffer = temp_buffer << 8;
    temp_buffer = temp_buffer | IMUdataBuffer[4];

    memcpy(&temp_float, &temp_buffer, sizeof(temp_buffer));
    IMU_yaw_measured = temp_float;

    temp_buffer = IMUdataBuffer[11];
    temp_buffer = temp_buffer << 8;
    temp_buffer = temp_buffer | IMUdataBuffer[10];
    temp_buffer = temp_buffer << 8;
    temp_buffer = temp_buffer | IMUdataBuffer[9];
    temp_buffer = temp_buffer << 8;
    temp_buffer = temp_buffer | IMUdataBuffer[8];

    memcpy(&temp_float, &temp_buffer, sizeof(temp_buffer));
    IMU_pitch_measured = temp_float;

    temp_buffer = IMUdataBuffer[15];
    temp_buffer = temp_buffer << 8;
    temp_buffer = temp_buffer | IMUdataBuffer[14];
    temp_buffer = temp_buffer << 8;
    temp_buffer = temp_buffer | IMUdataBuffer[13];
    temp_buffer = temp_buffer << 8;
    temp_buffer = temp_buffer | IMUdataBuffer[12];

    memcpy(&temp_float, &temp_buffer, sizeof(temp_buffer));
    IMU_roll_measured = temp_float;
	
	IMU_yaw = kalmanEstimate(IMU_yaw, estimateError_yaw, measurementError_yaw, IMU_yaw_measured);
	IMU_pitch = kalmanEstimate(IMU_pitch, estimateError_pitch, measurementError_pitch, IMU_pitch_measured);
	IMU_roll = kalmanEstimate(IMU_roll, estimateError_roll, measurementError_roll, IMU_roll_measured);
	
	estimateError_yaw = newESTerror(estimateError_yaw, measurementError_yaw);
	estimateError_pitch = newESTerror(estimateError_pitch, measurementError_pitch);
	estimateError_roll = newESTerror(estimateError_roll, measurementError_roll);
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
    SCI_setFIFOInterruptLevel(SCI_IMU_BASE, SCI_FIFO_TX5, SCI_FIFO_RX1);
    SCI_performSoftwareReset(SCI_IMU_BASE);

// clear buffer
    SCI_resetRxFIFO(SCI_IMU_BASE);

// enable SCI_RX PIE interrupt
    Interrupt_enable(INT_SCIB_RX);

// clear PIEACK
// see table 3-2 on pg.102 in tech. ref. manual for groups
    Interrupt_clearACKGroup(INTERRUPT_ACK_GROUP9);

    IMU_yaw = 69.420;
    IMU_pitch = 69.420;
    IMU_roll = 69.420;
    IMU_yaw_measured = 69.420;
    IMU_pitch_measured = 69.420;
    IMU_roll_measured = 69.420;
    
	estimateError_yaw = 2.0;
	measurementError_yaw = 4.0;
	estimateError_pitch = 2.0;
	measurementError_pitch = 4.0;
	estimateError_roll = 2.0;
	measurementError_roll = 4.0;
}

/**
 * initSCIwithFIFO(void) initializes the SCI module for use with
 * the FIFO buffer.
 */
void initIMUTransfer(void)
{
    // GPIO TX
    GPIO_setMasterCore(GPIO_SCITX_IMU, GPIO_CORE_CPU1);
    GPIO_setPinConfig(GPIO_CFG_SCITX_IMU);
    GPIO_setPadConfig(GPIO_SCITX_IMU, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(GPIO_SCITX_IMU, GPIO_DIR_MODE_OUT);
    GPIO_setQualificationMode(GPIO_SCITX_IMU, GPIO_QUAL_ASYNC);

    //GPIO RX
    GPIO_setMasterCore(GPIO_SCIRX_IMU, GPIO_CORE_CPU1);
    GPIO_setPinConfig(GPIO_CFG_SCIRX_IMU);
    GPIO_setPadConfig(GPIO_SCIRX_IMU, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(GPIO_SCIRX_IMU, GPIO_DIR_MODE_IN);
    GPIO_setQualificationMode(GPIO_SCIRX_IMU, GPIO_QUAL_ASYNC);

    // configure module
    // 8N1
    SCI_setConfig(
            SCI_IMU_BASE, DEVICE_LSPCLK_FREQ, SCI_IMU_BAUD,
            (SCI_CONFIG_WLEN_8 | SCI_CONFIG_STOP_ONE | SCI_CONFIG_PAR_NONE));
    SCI_enableModule(SCI_IMU_BASE);

    SCI_resetChannels(SCI_IMU_BASE);

    SCI_enableFIFO(SCI_IMU_BASE);
}

void initDebugTransfer(void)
{
    // GPIO TX
    GPIO_setPinConfig(GPIO_CFG_SCITX_DEBUG);
    GPIO_setPadConfig(GPIO_SCITX_DEBUG, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(GPIO_SCITX_DEBUG, GPIO_DIR_MODE_OUT);

    //GPIO RX
    GPIO_setPinConfig(GPIO_CFG_SCIRX_DEBUG);
    GPIO_setPadConfig(GPIO_SCIRX_DEBUG, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(GPIO_SCIRX_DEBUG, GPIO_DIR_MODE_IN);

    // configure module
    // 8N1
    SCI_setConfig(
            SCI_DEBUG_BASE, DEVICE_LSPCLK_FREQ, SCI_DEBUG_BAUD,
            (SCI_CONFIG_WLEN_8 | SCI_CONFIG_STOP_ONE | SCI_CONFIG_PAR_NONE));
    SCI_disableLoopback(SCI_DEBUG_BASE);
    SCI_performSoftwareReset(SCI_DEBUG_BASE);

    // FIFO
    SCI_disableFIFO(SCI_DEBUG_BASE);

    SCI_enableModule(SCI_DEBUG_BASE);
}

/**
 * initSCIwithFIFO(void) initializes the SCI module for use with
 * the FIFO buffer.
 */
void initSCI_IMUwithFIFO(void)
{
    // GPIO TX
    GPIO_setMasterCore(GPIO_SCITX_IMU, GPIO_CORE_CPU1);
    GPIO_setPinConfig(GPIO_CFG_SCITX_IMU);
    GPIO_setPadConfig(GPIO_SCITX_IMU, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(GPIO_SCITX_IMU, GPIO_DIR_MODE_OUT);
    GPIO_setQualificationMode(GPIO_SCITX_IMU, GPIO_QUAL_ASYNC);

    //GPIO RX
    GPIO_setMasterCore(GPIO_SCIRX_IMU, GPIO_CORE_CPU1);
    GPIO_setPinConfig(GPIO_CFG_SCIRX_IMU);
    GPIO_setPadConfig(GPIO_SCIRX_IMU, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(GPIO_SCIRX_IMU, GPIO_DIR_MODE_IN);
    GPIO_setQualificationMode(GPIO_SCIRX_IMU, GPIO_QUAL_ASYNC);

    // configure module
    // 8N1
    SCI_setConfig(
            SCI_IMU_BASE, DEVICE_LSPCLK_FREQ, SCI_IMU_BAUD,
            (SCI_CONFIG_WLEN_8 | SCI_CONFIG_STOP_ONE | SCI_CONFIG_PAR_NONE));
    SCI_enableModule(SCI_IMU_BASE);

    SCI_resetChannels(SCI_IMU_BASE);

    SCI_enableFIFO(SCI_IMU_BASE);
}

void SCIWriteInt(int intIn)
{
    /*char outputString[7];

    outputString[0] = (intIn / 10000) + '0';
    outputString[1] = ((intIn % 10000) / 1000) + '0';
    outputString[2] = ((intIn % 1000) / 100) + '0';
    outputString[3] = ((intIn % 100) / 10) + '0';
    outputString[4] = ((intIn % 10) / 1) + '0';
    outputString[5] = '\n';
    outputString[6] = '\r';*/

    char sign;

    if(intIn >= 0)
    {
        sign = '+';
    }
    else
    {
        sign = '-';
        intIn = intIn * -1;
    }

    char outputString[8];

    outputString[0] = sign;
    outputString[1] = (intIn / 10000) + '0';
    outputString[2] = ((intIn % 10000) / 1000) + '0';
    outputString[3] = ((intIn % 1000) / 100) + '0';
    outputString[4] = ((intIn % 100) / 10) + '0';
    outputString[5] = ((intIn % 10) / 1) + '0';
    outputString[6] = '\n';
    outputString[7] = '\r';

    SCIWriteChar(SCI_DEBUG_BASE, outputString, 8);
}

void SCIWriteChar(uint32_t SCIbase, const char * const dataArray,
                  int dataLength)
{

    SCI_writeCharArray(SCIbase, dataArray, dataLength);

}

