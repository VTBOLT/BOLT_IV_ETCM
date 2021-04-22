#ifndef IMU_ETCM_H_
#define IMU_ETCM_H_

#include "device.h"
#include "driverlib.h"
#include "leds_etcm.h"

void strobeIMUSyncIn(void);
void initIMUTransfer(void);
void initIMUSyncIn(void);
void initIMUInterrrupt(void);
float getIMUPitch(void);
float getIMURoll(void);
float getIMUYaw(void);
float getIMUAverageYaw(void);
float getIMUAveragePitch(void);
float getIMUAverageRoll(void);
void SCIWriteInt(int intIn);
void SCIWriteChar(uint32_t SCIbase, const char *const dataArray, int dataLength);
void initDebugTransfer(void);


//****************
// Defines
//****************
#define IMU_GPIO_CFG_SYNC_IN        GPIO_67_GPIO67
#define IMU_GPIO_SYNC_IN            67
#define IMU_FRAME_SIZE          18
#define IMU_GPIO_CFG_SCITX      GPIO_18_SCITXDB
#define IMU_GPIO_CFG_SCIRX      GPIO_19_SCIRXDB
#define IMU_GPIO_SCITX          18U
#define IMU_GPIO_SCIRX          19U


#define IMU_SCI_BASE            SCIB_BASE
#define IMU_SCI_BAUD            115200U

// DEBUG
#define DEBUG_GPIO_CFG_SCITX  GPIO_42_SCITXDA
#define DEBUG_GPIO_CFG_SCIRX  GPIO_43_SCIRXDA
#define DEBUG_GPIO_SCITX      42U
#define DEBUG_GPIO_SCIRX      43U
#define DEBUG_SCI_BASE        SCIA_BASE
#define DEBUG_SCI_BAUD        9600U


//**********
// Globals
//**********



#endif
