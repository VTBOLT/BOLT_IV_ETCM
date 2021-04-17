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
#include <imu_etcm.h>
#include <timer_etcm.h>
#include <leds_etcm.h>
#include <uart_etcm.h>
#include <inc/hw_ints.h>
#include <motor_control_etcm.h>
#include <speed_sensor_etcm.h>
#include <suspension_sensor_etcm.h>
#include <throttle_input_etcm.h>


// Function Prototypes
void run(void);
void CANtest(void);
void LEDflash(void);
void init(void);
void initLookup(void);
void initGPIO(void);
void initInterrupts(void);

//Variables
const float IMU_PITCH_CORRECTION = -2.0;
const float IMU_ROLL_CORRECTION = 0.0;
const float IMU_YAW_CORRECTION = 129.0;

void main(void)
{
    init();
    run();
}


//Main, calls init and run
void run(void)
{
//    float torque_request = 0; // likely to change type
//    // start the timer
//    startTimer0();
//
//    float pitch;
//    float roll;
//    float yaw;
//    while (1)
//    {
//        // Pull in sensor data to local variables
//        // This will use getters inside the peripheral .h/.c files
//
//        // @todo - implement ADC and write getThrottle() method. Verify values with separate ADC and ensure full range of input works
//        //         This could probably be done by extending Quinton's ADC code in the position_sensor branch
//        float rawThrottleIn = getThrottleVoltage(); // getThrottle()
//
//        torque_request = rawThrottleIn; // unmodified
//
//        /* No TC, unnecessary
//        int rawFrontSusp = 0; // getFrontSusp()
//        int rawBackSusp = 0; // getBackSusp()
//        *
//        *
//        * Follow lookup table logic
//        * Specific logic TBD
//        *
//        * The basic structure will be as follows:
//        *      First, check which TC categories need to be calculated/looked up
//        *      Then, do those calculations and store the results
//        *      Look at all the results and find the largest reduction
//        *      Move forward with that
//        *
//        *
//        *bool checks[4]; // raw_in, traction_control, anti_wheelie, anti_jerk
//        *
//        *checks[0] = true; // raw_in will always qualify
//        *
//        *
//        * Carry out any calculations
//        */
//
//
//        // Send torque request to motor - this will be the raw torque request, unmodified
////        requestTorque(3.00); // @todo - verify functionality in DAC file over full range of output values, Stephen wrote this and it's merged
//        setDACOutputVoltage(torque_request);
//
//        // Send a test CANmsg
//        /* @todo - you'll still want CAN messages out of this so that the CANlogger can pick
//         * them up and you can record and verify that IO throttle matches up. See can_etcm.h
//         * and can_etcm.c, written by Tyler. For any further functionality, implement from
//         * the CAN examples. They can be finicky and you should make sure there aren't any
//         * jumpers or other hardware on the silkscreen that need to be arranged in a certain
//         * way, but the examples will work. Also, consider buying a CAN analyzer.
//        //CANtest()
//         * @todo - verify test works, then put in actual data
//         */
//
//        // Flash the blue LED - this is just debug/verify the board is actually functioning
////        LEDflash();
//
////        getIMUdata(); // @todo - this is Quinton's code, I'm fairly sure that it works, still verify
//
//        //getIMUdata();
////        strobeIMUSyncIn();
////
////        pitch = getIMUPitch() + IMU_PITCH_CORRECTION;
////        roll = getIMURoll() + IMU_ROLL_CORRECTION;;
////        yaw = getIMUYaw() + IMU_YAW_CORRECTION;;
////
////        SCIWriteChar(SCI_DEBUG_BASE, "IMU Pitch: ", 11);
////        SCIWriteInt((int) pitch*100);
////
////        SCIWriteChar(SCI_DEBUG_BASE, "IMU Roll: ", 10);
////        SCIWriteInt((int) roll*100);
////
////        SCIWriteChar(SCI_DEBUG_BASE, "IMU Yaw: ", 9);
////        SCIWriteInt((int) yaw*100);
//
//        SCIWriteChar(SCI_DEBUG_BASE, "\n\r\n\r\n\r", 6);
//        SCIWriteChar(SCI_DEBUG_BASE, "Front RPM: ", 11);
//        SCIWriteInt(getRPMFront());
//        SCIWriteChar(SCI_DEBUG_BASE, "Rear RPM: ", 10);
//        SCIWriteInt(getRPMRear());
//
//
//        // after 5 seconds, reduce period to 500mS
//        if (cpuTimer0IntCount >= 5)
//        {
//            reloadTimer0(500);
//        }
//    }

    while (1)
    {
        ADC_forceSOC(ADCB_BASE, ADC_SOC_NUMBER0);
        // Pull in sensor data to local variables
        // This will use getters inside the peripheral .h/.c files
    }
}

//Initialize, runs all initialization functions
void init(void)
{
    // Initialize device clock and peripherals
    Device_init();
    initGPIO();     // do not move

    // Interrupt
    Interrupt_initModule();
    Interrupt_initVectorTable();

    initLEDS();
    //initLookup(); // removed, no lookup table
    initSpeedSensorEPWM();   // should be removed, only TC-required
    initSpeedSensorECAP();

//    initThrottleADC();
//    initThrottleADCSOC();
    initFrontSuspensionSensor();
    initRearSuspensionSensor();
    CANA_init();
    //initSCI();    // @todo - figure out which or both initSCI methods are necessary
    initSCIFIFO();
//    initDAC();
    initTimer0();
    initIMUTransfer();
    initDebugTransfer();
    initInterrupts();
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

void logCAN(void) {
    uint16_t mailbox = 1; // Valid mailboxes range from 1-32

    float frontRpm = getRPMFront();
    float rearRpm = getRPMRear();
    float frontSuspPos = getFrontSensorPosition();
    float rearSuspPos = getRearSensorPosition();
    float yaw = getIMUYaw();
    float pitch = getIMUPitch();
    float roll = getIMURoll();

    // Message should be 8 *bytes* in length;
    CANA_transmitFloat(&frontRpm, mailbox++);
    CANA_transmitFloat(&rearRpm, mailbox++);
    CANA_transmitFloat(&frontSuspPos, mailbox++);
    CANA_transmitFloat(&rearSuspPos, mailbox++);
    CANA_transmitFloat(&yaw, mailbox++);
    CANA_transmitFloat(&pitch, mailbox++);
    CANA_transmitFloat(&roll, mailbox++);
}

void LEDflash(void){
    GPIO_writePin(GPIO_BLUE_LED, 1);
    // pause for a bit
    unsigned long index = 0; // why do I have to declare this here?
    for (index = 0; index <= 1000000; index++)
        ;
    GPIO_writePin(GPIO_BLUE_LED, 0);
    for (index = 0; index <= 1000000; index++)
        ;
}

void initInterrupts(void)
{
    // Initialize PIE and clear PIE registers. Disables CPU interrupts.
//    Interrupt_initModule();
    // Initialize the PIE vector table with pointers to the shell Interrupt
//    Interrupt_initVectorTable();

    // Interrupt init calls go here
    initIMUinterrupt();
    initTimer0Interrupt();
    initSpeedSensorInterrupts();

    // Allows CPU to respond to interrupts - this should always be last - see Pg. 100 of Tech Ref Manual
    Interrupt_enableMaster();
    // Enable interrupts
    EINT;
    // Enable debug events
    ERTM;
}

/**
 * Module GPIO inits are in their respective .c file.
 */
void initGPIO(void)
{
    Device_initGPIO();      // must be called first?

    // SYNC_IN
    GPIO_setPinConfig(IMU_GPIO_CFG_SYNC_IN);
    GPIO_setPadConfig(IMU_GPIO_SYNC_IN, GPIO_PIN_TYPE_STD);
    GPIO_setDirectionMode(IMU_GPIO_SYNC_IN, GPIO_DIR_MODE_OUT);
    GPIO_writePin(IMU_GPIO_SYNC_IN, 0); // default state

    initSpeedSensorGPIO();
}
